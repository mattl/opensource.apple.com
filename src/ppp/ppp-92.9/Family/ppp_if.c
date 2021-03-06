/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

/* -----------------------------------------------------------------------------
*
*  Theory of operation :
*
*  this file implements the interface driver for the ppp family
*
*  it's the responsability of the driver to update the statistics
*  whenever that makes sense
*     ifnet.if_lastchange = a packet is present, a packet starts to be sent
*     ifnet.if_ibytes = nb of correct PPP bytes received (does not include escapes...)
*     ifnet.if_obytes = nb of correct PPP bytes sent (does not include escapes...)
*     ifnet.if_ipackets = nb of PPP packet received
*     ifnet.if_opackets = nb of PPP packet sent
*     ifnet.if_ierrors = nb on input packets in error
*     ifnet.if_oerrors = nb on ouptut packets in error
*
----------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------
Includes
----------------------------------------------------------------------------- */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/malloc.h>
#include <sys/syslog.h>
#include <sys/sockio.h>
#include <sys/kernel.h>
#include <machine/spl.h>
#include <kern/clock.h>

#include <net/if_types.h>
#include <net/dlil.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <net/bpf.h>

#include "slcompress.h"
#include "ppp_defs.h"		// public ppp values
#include "if_ppp.h"		// public ppp API
#include "if_ppplink.h"		// public link API
#include "ppp_if.h"
#include "ppp_domain.h"
#include "ppp_ip.h"
#include "ppp_compress.h"
#include "ppp_comp.h"
#include "ppp_link.h"


/* -----------------------------------------------------------------------------
Definitions
----------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------
Forward declarations
----------------------------------------------------------------------------- */

static int	ppp_if_output(struct ifnet *ifp, struct mbuf *m);
static int	ppp_if_if_free(struct ifnet *ifp);
static int 	ppp_if_ioctl(struct ifnet *ifp, u_long cmd, void *data);

static int 	ppp_if_detach(struct ifnet *ifp);
static struct ppp_if *ppp_if_findunit(u_short unit);
static u_short 	ppp_if_findfreeunit();

/* -----------------------------------------------------------------------------
Globals
----------------------------------------------------------------------------- */

static TAILQ_HEAD(, ppp_if) 	ppp_if_head;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
int ppp_if_init()
{
    TAILQ_INIT(&ppp_if_head);
    return 0;
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
int ppp_if_dispose()
{
    struct ppp_if  	*wan;

    // can't dispose if interface are in use
    TAILQ_FOREACH(wan, &ppp_if_head, next)
        if (wan->in_use)
            return EBUSY;
            
    while (wan = TAILQ_FIRST(&ppp_if_head)) {
        TAILQ_REMOVE(&ppp_if_head, wan, next);
    	FREE(wan, M_DEVBUF);
    }
    
    return 0;
}

/* -----------------------------------------------------------------------------
find an unused ifnet in the list of interfaces
----------------------------------------------------------------------------- */
struct ppp_if *ppp_if_findunused()
{
    struct ppp_if  	*wan;

    TAILQ_FOREACH(wan, &ppp_if_head, next) 
        if (!wan->in_use)
            return wan;
        
    return 0;
}

/* -----------------------------------------------------------------------------
detach pppoe interface dlil layer
----------------------------------------------------------------------------- */
int ppp_if_attach(u_short *unit)
{
    int 		ret;	
    struct ppp_if  	*wan;
    
    wan = ppp_if_findunused();
    if (wan) // reinit the structure
        TAILQ_REMOVE(&ppp_if_head, wan, next);
    else // alloc a new structure
        MALLOC(wan, struct ppp_if *, sizeof(struct ppp_if), M_DEVBUF, M_WAITOK);
    if (!wan)
        return ENOMEM;

    bzero(wan, sizeof(struct ppp_if));

    TAILQ_INSERT_TAIL(&ppp_if_head, wan, next);

    // check if number requested is already in use
    if ((*unit != 0xFFFF) && ppp_if_findunit(*unit))
        return EINVAL;

    // it's time now to register our brand new channel
    wan->net.if_name 		= APPLE_PPP_NAME;
    wan->net.if_family 	= APPLE_IF_FAM_PPP;
    wan->net.if_mtu 		= PPP_MTU;
    wan->net.if_flags 	= IFF_POINTOPOINT | IFF_MULTICAST; // || IFF_RUNNING
    wan->net.if_type 		= IFT_PPP;
    wan->net.if_hdrlen 	= PPP_HDRLEN;
    wan->net.if_ioctl 	= ppp_if_ioctl;
    wan->net.if_output 	= ppp_if_output;
    wan->net.if_free 		= ppp_if_if_free;
    wan->net.if_baudrate 	= 0; // 10 Mbits/s ???
    getmicrotime(&wan->net.if_lastchange);
    TAILQ_INIT(&wan->link_head);

    wan->net.if_unit = *unit != 0xFFFF ? *unit : ppp_if_findfreeunit();
    wan->in_use = 1;
    *unit = wan->net.if_unit;
    ret = dlil_if_attach(&wan->net);
    
    bpfattach(&wan->net, DLT_PPP, PPP_HDRLEN);

    return 0;
}

/* -----------------------------------------------------------------------------
detach ppp interface from dlil layer
----------------------------------------------------------------------------- */
int ppp_if_detach(struct ifnet *ifp)
{
    struct ppp_if  	*wan = (struct ppp_if *)ifp;
    int 		ret;
    struct ppp_link	*link;

    // need to remove all ref to ifnet in link structures
    TAILQ_FOREACH(link, &wan->link_head, lk_bdl_next) {
        // do we need a free function ?
        link->lk_ifnet = 0;
    }

    ppp_comp_close(wan);

    ppp_ip_detach(ifp);
    if (wan->vjcomp) {
	FREE(wan->vjcomp, M_DEVBUF);
	wan->vjcomp = 0;
    }


    ret = dlil_if_detach(ifp);
    switch (ret) {
        case 0:
            break;
        case DLIL_WAIT_FOR_FREE:
            sleep(ifp, PZERO+1);
            break;
        default:
            return KERN_FAILURE;
    }


    // don't free the interface
    wan->in_use = 0;    
    // TAILQ_REMOVE(&ppp_if_head, wan, next);
    // FREE(ifp, M_DEVBUF);

    return 0;
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
int ppp_if_attachclient(u_short unit, void *host, struct ifnet **ifp)
{
    struct ppp_if  	*wan;

    TAILQ_FOREACH(wan, &ppp_if_head, next) {
        if (wan->in_use && wan->net.if_unit == unit) {
            *ifp = (struct ifnet *)wan;
            if (!wan->host)    // don't override the first attachment (use a list ?)
                wan->host = host;
             wan->nbclients++;   
            return 0;
        }
    }
    return ENODEV;
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void ppp_if_detachclient(struct ifnet *ifp, void *host)
{
    struct ppp_if  	*wan = (struct ppp_if *)ifp;

    if (wan->host) {
        if (wan->host == host)
            wan->host = 0;
        wan->nbclients--;
        if (!wan->nbclients)
            ppp_if_detach(ifp);
    }
}

/* -----------------------------------------------------------------------------
find a free unit in the interface list
----------------------------------------------------------------------------- */
u_short ppp_if_findfreeunit()
{
    struct ppp_if  	*wan = TAILQ_FIRST(&ppp_if_head);
    u_short 		unit = 0;

    while (wan) {
    	if (wan->in_use && wan->net.if_unit == unit) {
            unit++;
            wan = TAILQ_FIRST(&ppp_if_head); // restart
        }
        else 
            wan = TAILQ_NEXT(wan, next); // continue
    }
    return unit;
}

/* -----------------------------------------------------------------------------
find a the unit number in the interface list
----------------------------------------------------------------------------- */
struct ppp_if *ppp_if_findunit(u_short unit)
{
    struct ppp_if  	*wan;

    TAILQ_FOREACH(wan, &ppp_if_head, next) {
        if (wan->in_use && wan->net.if_unit == unit)
            return wan; 
    }
    return NULL;
}

/* -----------------------------------------------------------------------------
called from pppoe_rfc when data are present
----------------------------------------------------------------------------- */
int ppp_if_input(struct ifnet *ifp, struct mbuf *m, u_int16_t proto, u_int16_t hdrlen)
{    
    struct ppp_if 	*wan = (struct ppp_if *)ifp;
    int 		inlen, vjlen;
    u_char		*iphdr, *p = mtod(m, u_char *);
    u_int 		hlen;

    m->m_pkthdr.header = p;		// header point to the protocol header (0x21 or 0x0021)
    m_adj(m, hdrlen);			// the packet points to the real data (0x45)
    p = mtod(m, u_char *);

    if (wan->sc_flags & SC_DECOMP_RUN) {
        switch (proto) {
            case PPP_COMP:
                if (ppp_comp_decompress(wan, &m) != DECOMP_OK) {
                    LOGDBG(ifp, (LOGVAL, "ppp%d: decompression error\n", ifp->if_unit));
                    goto free; 
                }
                p = mtod(m, u_char *);
                proto = p[0];
                hdrlen = 1;
                if (!(proto & 0x1)) {  // lowest bit set for lowest byte of protocol
                    proto = (proto << 8) + p[1];
                    hdrlen = 2;
                } 
                m->m_pkthdr.header = p;// header point to the protocol header (0x21 or 0x0021)
                m_adj(m, hdrlen);	// the packet points to the real data (0x45)
                p = mtod(m, u_char *);
                break;
            default:
                ppp_comp_incompress(wan, m);
        }
    }

    switch (proto) {
        case PPP_VJC_COMP:
        case PPP_VJC_UNCOMP:
            
            if (!(wan->sc_flags & SC_COMP_TCP))
                goto reject;
        
            if (!wan->vjcomp) {
                LOGDBG(ifp, (LOGVAL, "ppp%d: VJ structure not allocated\n", ifp->if_unit));
                goto free; 
            }
    
            inlen = m->m_pkthdr.len;
            
            if (proto == PPP_VJC_COMP) {

                vjlen = sl_uncompress_tcp_core(p, m->m_len, inlen, TYPE_COMPRESSED_TCP,
                    wan->vjcomp, &iphdr, &hlen);

                if (vjlen <= 0) {
                    LOGDBG(ifp, (LOGVAL, "ppp%d: VJ uncompress failed on type PPP_VJC_COMP\n", ifp->if_unit));
                    goto free; 
                }

                // we must move data in the buffer, to add the uncompressed TCP/IP header...
                if (M_TRAILINGSPACE(m) < (hlen - vjlen)) {
                    goto free; 
                }
                bcopy(p + vjlen, p + hlen, inlen - vjlen);
                bcopy(iphdr, p, hlen);
                m->m_len += hlen - vjlen;
                m->m_pkthdr.len += hlen - vjlen;
            }
            else {
                vjlen = sl_uncompress_tcp_core(p, m->m_len, inlen, TYPE_UNCOMPRESSED_TCP, 
                    wan->vjcomp, &iphdr, &hlen);

                if (vjlen < 0) {
                    LOGDBG(ifp, (LOGVAL, "ppp%d: VJ uncompress failed on type TYPE_UNCOMPRESSED_TCP\n", ifp->if_unit));
                    goto free; 
                }
            }
            *(u_char *)m->m_pkthdr.header = PPP_IP; // change the protocol, use 1 byte
            proto = PPP_IP;
            //no break;
        case PPP_IP:
            if (wan->npmode[NP_IP] != NPMODE_PASS)
                goto reject;
            break;
        case PPP_CCP:
            ppp_comp_ccp(wan, m, 1);
            goto reject;
        default:
            goto reject;
    }

    // See if bpf wants to look at the packet.
    if (ifp->if_bpf && (M_LEADINGSPACE(m) >= 4)) {
        M_PREPEND(m, 4, M_WAIT);
        p = mtod(m, u_char *);
        *(u_int16_t *)p = 0xFF03;
        *(u_int16_t *)(p + 2) = proto;
        bpf_mtap(ifp, m);
        m_adj(m, 4);
    }

    getmicrotime(&ifp->if_lastchange);
    ifp->if_ibytes += m->m_pkthdr.len;
    ifp->if_ipackets++;
    m->m_pkthdr.rcvif = ifp;
    wan->last_recv = clock_get_system_value().tv_sec;

    dlil_input(ifp, m, m);
    return 0;
    
reject:

    // unexpected network protocol
    M_PREPEND(m, hdrlen, M_DONTWAIT);	// just reput the header before to send it to pppd
    ppp_proto_input(wan->host, m);
    return 0;

free:
    m_free(m);
    ifp->if_ierrors++;
    return ENOMEM;
}

/* -----------------------------------------------------------------------------
This gets called when the interface is freed
(if dlil_if_detach has returned DLIL_WAIT_FOR_FREE)
----------------------------------------------------------------------------- */
int ppp_if_if_free(struct ifnet *ifp)
{
    wakeup(ifp);
    return 0;
}

/* -----------------------------------------------------------------------------
Process an ioctl request to the ppp interface
----------------------------------------------------------------------------- */
int ppp_if_control(struct ifnet *ifp, u_long cmd, void *data)
{
    struct ppp_if 	*wan = (struct ppp_if *)ifp;
    int 		error = 0, npx;
    u_int16_t		mru;
    u_int32_t		flags;
    u_int32_t		t;
    struct npioctl 	*npi;
    
    //LOGDBG(ifp, (LOGVAL, "ppp_if_control, (ifnet = %s%d), cmd = 0x%x\n", ifp->if_name, ifp->if_unit, cmd));

    switch (cmd) {
	case PPPIOCSDEBUG:
            flags = *(int *)data;
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCSDEBUG (level = 0x%x)\n", flags));
            ifp->if_flags &= ~(IFF_DEBUG + PPP_LOG_INPKT + PPP_LOG_OUTPKT);  
            if (flags & 1) ifp->if_flags |= IFF_DEBUG;	// general purpose debugging
            if (flags & 2) ifp->if_flags |= PPP_LOG_INPKT;	// trace all packets in
            if (flags & 4) ifp->if_flags |= PPP_LOG_OUTPKT;	// trace all packets out
            break;

	case PPPIOCGDEBUG:
            flags = 0;
            if (ifp->if_flags & IFF_DEBUG) flags |= 1;
            if (ifp->if_flags & PPP_LOG_INPKT) flags |= 2;
            if (ifp->if_flags & PPP_LOG_OUTPKT) flags |= 4;
            *(int *)data = flags;
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCGDEBUG (level = 0x%x)\n", flags));
            break;

	case PPPIOCSMRU:
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCSMRU\n"));
            mru = *(int *)data;
            wan->mru = mru;
            break;

	case PPPIOCSFLAGS:
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCSFLAGS, old flags = 0x%x new flags = 0x%x, \n", wan->sc_flags, (wan->sc_flags & ~SC_MASK) | flags));
            flags = *(int *)data & SC_MASK;
            wan->sc_flags = (wan->sc_flags & ~SC_MASK) | flags;
            break;

	case PPPIOCGFLAGS:
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCGFLAGS\n"));
            *(int *)data = wan->sc_flags;
            break;

	case PPPIOCSCOMPRESS:
            error = ppp_comp_setcompressor(wan, data);
            break;

	case PPPIOCGUNIT:
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCGUNIT\n"));
            *(int *)data = wan->net.if_unit;
            break;

	case PPPIOCGIDLE:
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCGIDLE\n"));
            t = clock_get_system_value().tv_sec;
            ((struct ppp_idle *)data)->xmit_idle = t - wan->last_xmit;
            ((struct ppp_idle *)data)->recv_idle = t - wan->last_recv;
            break;

        case PPPIOCSMAXCID:
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCSMAXCID\n"));
            // allocate the vj structure first
            if (!wan->vjcomp) {
                MALLOC(wan->vjcomp, struct slcompress *, sizeof(struct slcompress), 
                    M_DEVBUF, M_WAITOK); 	
                if (!wan->vjcomp) 
                    return ENOMEM;
                sl_compress_init(wan->vjcomp, -1);
            }
            // reeinit the compressor
            sl_compress_init(wan->vjcomp, *(int *)data);
            break;

	case PPPIOCSNPMODE:
	case PPPIOCGNPMODE:
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: PPPIOCSNPMODE/PPPIOCGNPMODE\n"));
            npi = (struct npioctl *) data;
            switch (npi->protocol) {
                case PPP_IP:
                    npx = NP_IP;
                    break;
                default:
                    return EINVAL;
            }
            if (cmd == PPPIOCGNPMODE) {
                npi->mode = wan->npmode[npx];
            } else {
                if (npi->mode != wan->npmode[npx]) {
                    wan->npmode[npx] = npi->mode;
                    if (npi->mode != NPMODE_QUEUE) {
                        //ppp_requeue(sc);
                        //(*sc->sc_start)(sc);
                    }
                }
            }
            break;

	default:
            LOGDBG(ifp, (LOGVAL, "ppp_if_control: unknown ioctl\n"));
            error = EINVAL;
	}

    return error;
}

/* -----------------------------------------------------------------------------
Process an ioctl request to the ppp interface
----------------------------------------------------------------------------- */
int ppp_if_ioctl(struct ifnet *ifp, u_long cmd, void *data)
{
    struct ppp_if 	*wan = (struct ppp_if *)ifp;
    struct ifreq 	*ifr = (struct ifreq *)data;
    struct ifaddr 	*ifa = (struct ifaddr *)data;
    int 		error = 0;
    struct ppp_stats 	*psp;
    struct in_ifaddr 	*ia = (struct in_ifaddr *)data;

    //LOGDBG(ifp, (LOGVAL, "ppp_if_ioctl, cmd = 0x%x\n", cmd));
    if (!wan->in_use)
        return EFAULT;

    switch (cmd) {

        case SIOCSIFFLAGS:
            LOGDBG(ifp, (LOGVAL, "ppp_if_ioctl: cmd = SIOCSIFFLAGS\n"));
            // even if this case does nothing, it must be there to return 0
            //if ((ifp->if_flags & IFF_RUNNING) == 0)
            //    ifp->if_flags &= ~IFF_UP;
            break;

        case SIOCSIFADDR:
        case SIOCAIFADDR:
            LOGDBG(ifp, (LOGVAL, "ppp_if_ioctl: cmd = SIOCSIFADDR/SIOCAIFADDR\n"));
            switch(ifa->ifa_addr->sa_family) {
                case AF_INET:
                    // plumb now the ip protocol
                    error = ppp_ip_attach(ifp, (struct sockaddr_in *)ifa->ifa_addr, &ia->ia_ifa.ifa_dlt);
                    break;
                default:
                    error = EAFNOSUPPORT;
                    break;
            }
            break;

        case SIOCDIFADDR:
            LOGDBG(ifp, (LOGVAL, "ppp_if_ioctl: cmd = SIOCDIFADDR\n"));
            break;

        case SIOCSIFDSTADDR:
            LOGDBG(ifp, (LOGVAL, "ppp_if_ioctl: cmd = SIOCSIFDSTADDR\n"));
            break;

	case SIOCGPPPSTATS:
            LOGDBG(ifp, (LOGVAL, "ppp_if_ioctl, SIOCGPPPSTATS\n"));
            psp = &((struct ifpppstatsreq *) data)->stats;
            bzero(psp, sizeof(*psp));

            psp->p.ppp_ibytes = ifp->if_ibytes;
            psp->p.ppp_obytes = ifp->if_obytes;
            psp->p.ppp_ipackets = ifp->if_ipackets;
            psp->p.ppp_opackets = ifp->if_opackets;
            psp->p.ppp_ierrors = ifp->if_ierrors;
            psp->p.ppp_oerrors = ifp->if_oerrors;

#if 0
            if (sc->sc_comp) {
                psp->vj.vjs_packets = sc->sc_comp->sls_packets;
                psp->vj.vjs_compressed = sc->sc_comp->sls_compressed;
                psp->vj.vjs_searches = sc->sc_comp->sls_searches;
                psp->vj.vjs_misses = sc->sc_comp->sls_misses;
                psp->vj.vjs_uncompressedin = sc->sc_comp->sls_uncompressedin;
                psp->vj.vjs_compressedin = sc->sc_comp->sls_compressedin;
                psp->vj.vjs_errorin = sc->sc_comp->sls_errorin;
                psp->vj.vjs_tossed = sc->sc_comp->sls_tossed;
            }
#endif
            break;

        case SIOCSIFMTU:
            LOGDBG(ifp, (LOGVAL, "ppp_if_ioctl, SIOCSIFMTU\n"));
            // should we check the minimum MTU for all channels attached to that interface ?
            if (ifr->ifr_mtu > PPP_MTU)
                error = EINVAL;
            else
                ifp->if_mtu = ifr->ifr_mtu;
            break;

	default:
            LOGDBG(ifp, (LOGVAL, "ppp_if_ioctl, unknown ioctl, cmd = 0x%x\n", cmd));
            error = EINVAL;
	}

    return error;
}

/* -----------------------------------------------------------------------------
This gets called at splnet from if_ppp.c at various times
when there is data ready to be sent
----------------------------------------------------------------------------- */
int ppp_if_output(struct ifnet *ifp, struct mbuf *m)
{
    struct ppp_if 	*wan = (struct ppp_if *)ifp;
    int 		error;
    u_int16_t		proto;
    enum NPmode		mode;
    
    if (!wan->in_use) {
        error = EFAULT;
        goto bad;
    }

    proto = *mtod(m, u_short *);
    switch (proto) {
        case PPP_IP:
            mode = wan->npmode[NP_IP];
            break;
        default:
            // should never happen, since we attached the protocol ourself
            error = EAFNOSUPPORT;
            goto bad;
    }

    switch (mode) {
        case NPMODE_ERROR:
            error = ENETDOWN;
            goto bad;
        case NPMODE_QUEUE:
        case NPMODE_DROP:
            error = 0;
            goto bad;
        case NPMODE_PASS:
            break;
    }

    // See if bpf wants to look at the packet.
    if (ifp->if_bpf && (M_LEADINGSPACE(m) >= 2)) {
        M_PREPEND(m, 2, M_WAIT);
        *mtod(m, u_int16_t *) = 0xFF03;
        bpf_mtap(ifp, m);
        m_adj(m, 2);
    }

    // Update interface statistics.
    getmicrotime(&ifp->if_lastchange);
    wan->last_xmit = clock_get_system_value().tv_sec;
    ifp->if_obytes += m->m_pkthdr.len - 2; // don't count protocol header
    ifp->if_opackets++;

    if (wan->sc_flags & SC_LOOP_TRAFFIC) {
        ppp_proto_input(wan->host, m);
        return 0;
    }
        
    ppp_if_send(ifp, m);
    return 0;

bad:
    m_freem(m);
    ifp->if_oerrors++;
    return error;
}

/* -----------------------------------------------------------------------------
 * Connect a PPP channel to a PPP interface unit.
----------------------------------------------------------------------------- */
int ppp_if_attachlink(struct ppp_link *link, int unit)
{
    struct ppp_if 	*wan;

    if (link->lk_ifnet)	// already attached
        return EINVAL;

    wan = ppp_if_findunit(unit);
    if (!wan)
	return EINVAL;

    wan->net.if_flags |= IFF_RUNNING;
    wan->net.if_baudrate += link->lk_baudrate;

    TAILQ_INSERT_TAIL(&wan->link_head, link, lk_bdl_next);
    wan->nblinks++;
    link->lk_ifnet = (struct ifnet *)wan;

    return 0;
}

/* -----------------------------------------------------------------------------
 * Disconnect a channel from its ppp unit.
----------------------------------------------------------------------------- */
int ppp_if_detachlink(struct ppp_link *link)
{
    struct ppp_if 	*wan = (struct ppp_if *)link->lk_ifnet;

    if (!wan)
        return EINVAL; // link already detached

    // check if this is the last link, when multilink is coded
    wan->net.if_flags &= ~IFF_RUNNING;
    wan->net.if_baudrate -= link->lk_baudrate;
    
    TAILQ_REMOVE(&wan->link_head, link, lk_bdl_next);
    wan->nblinks--;
    link->lk_ifnet = 0;
    return 0;
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
int ppp_if_send(struct ifnet *ifp, struct mbuf *m)
{
    struct ppp_if 	*wan = (struct ppp_if *)ifp;
    u_int16_t		proto = *mtod(m, u_int16_t *);	// always the 2 first bytes
        
    if (IF_QFULL(&ifp->if_snd)) {
        IF_DROP(&ifp->if_snd);
        ifp->if_oerrors++;
        return ENOBUFS;
    }

    switch (proto) {
        case PPP_IP:
            // see if we can compress it
            if ((wan->sc_flags & SC_COMP_TCP) && wan->vjcomp) {
                struct mbuf 	*mp = m;
                struct ip 	*ip = (struct ip *) (mtod(m, u_char *) + 2);
                int 		vjtype;
                
                // skip mbuf, in case the ppp header and ip header are not in the same mbuf
                if (mp->m_len <= 2) {
                    mp = mp->m_next;
                    if (!mp)
                        break;
                    ip = mtod(mp, struct ip *);
                }
                // this code assumes the IP/TCP header is in one non-shared mbuf 
                if (ip->ip_p == IPPROTO_TCP) {
                    vjtype = sl_compress_tcp(mp, ip, wan->vjcomp, !(wan->sc_flags & SC_NO_TCP_CCID));
                    switch (vjtype) {
                        case TYPE_UNCOMPRESSED_TCP:
                            *mtod(m, u_int16_t *) = PPP_VJC_UNCOMP; // update protocol
                            break;
                        case TYPE_COMPRESSED_TCP:
                            *mtod(m, u_int16_t *) = PPP_VJC_COMP; // header has moved, update protocol
                        break;
                    }
                }
            }
            break;
        case PPP_CCP:
            m_adj(m, 2);
            ppp_comp_ccp(wan, m, 0);
            M_PREPEND(m, 2, M_DONTWAIT);
            break;
    }

    if (wan->sc_flags & SC_COMP_RUN) {

        if (ppp_comp_compress(wan, &m) == COMP_OK) {
            M_PREPEND(m, 2, M_DONTWAIT);
            if (m == 0) {
                ifp->if_oerrors++;
                return ENOBUFS;
            }
            *mtod(m, u_int16_t *) = PPP_COMP; // update protocol
        } 
    } 

    if (ifp->if_snd.ifq_len) {
        IF_ENQUEUE(&ifp->if_snd, m);
    }
    else 
       ppp_if_xmit(ifp, m);
    
    return 0;
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
int ppp_if_xmit(struct ifnet *ifp, struct mbuf *m)
{
    struct ppp_if 	*wan = (struct ppp_if *)ifp;
    struct ppp_link	*link;
    int 		err, len;
            
    if (m == 0)
        IF_DEQUEUE(&ifp->if_snd, m);

    while (m) {

        link = TAILQ_FIRST(&wan->link_head);
        if (link == 0) {
            LOGDBG(ifp, (LOGVAL, "ppp%d: Trying to send data with link detached\n", ifp->if_unit));
            // just flush everything
            getmicrotime(&ifp->if_lastchange);
            while (m) {
                ifp->if_oerrors++;
                m_freem(m);
                IF_DEQUEUE(&ifp->if_snd, m);
            };
            return 1;
        }
    
        if (link->lk_flags & SC_HOLD) {
            // should try next link
            m_freem(m);
            IF_DEQUEUE(&ifp->if_snd, m);
            continue;
        }

        if (link->lk_flags & (SC_XMIT_BUSY | SC_XMIT_FULL)) {
            // should try next link
            IF_PREPEND(&ifp->if_snd, m);
            return 0;
        }

        // get the len before we send the packet, 
        // we can not assume the state of the mbuf when we return
        len = m->m_pkthdr.len;

        // since we tested the lk_flags, ppp_link_send should not failed
        // except if there is a dramatic error
        link->lk_flags |= SC_XMIT_BUSY;
        err = ppp_link_send(link, m);
        link->lk_flags &= ~SC_XMIT_BUSY;
        if (err) {
            // should try next link
            IF_PREPEND(&ifp->if_snd, m);
            return err;
        }

        IF_DEQUEUE(&ifp->if_snd, m);
    }
     
    return 0;
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void ppp_if_error(struct ifnet *ifp)
{
    struct ppp_if 	*wan = (struct ppp_if *)ifp;
        
    // reset vj compression
    if (wan->vjcomp) {
	sl_uncompress_tcp(NULL, 0, TYPE_ERROR, wan->vjcomp);
    }
}
