#	from @(#)Makefile.inc	8.2 (Berkeley) 9/5/93
# $FreeBSD: src/lib/libc/net/Makefile.inc,v 1.43 2001/10/23 06:22:14 imp Exp $

# machine-independent net sources
.PATH: ${.CURDIR}/${MACHINE_ARCH}/net ${.CURDIR}/net

.include "Makefile.fbsd_begin"
FBSDMISRCS= addr2ascii.c ascii2addr.c inet_addr.c inet_lnaof.c \
	inet_makeaddr.c inet_net_ntop.c inet_net_pton.c inet_neta.c \
	inet_netof.c inet_network.c inet_ntoa.c linkaddr.c nsap_addr.c \
	recv.c send.c
.include "Makefile.fbsd_end"

UNIX03SRCS+= recv.c send.c

CFLAGS+= -I${.OBJDIR}

# machine-dependent net sources
.if exists(${.CURDIR}/${MACHINE_ARCH}/net/Makefile.inc)
.include "${.CURDIR}/${MACHINE_ARCH}/net/Makefile.inc"
.endif

.if ${LIB} == "c"
MAN3+=	byteorder.3 ethers.3 hesiod.3 \
	rcmdsh.3 resolver.3
.include "Makefile.fbsd_begin"
FBSDMAN3= addr2ascii.3 inet.3 inet_net.3 linkaddr.3
.include "Makefile.fbsd_end"

MLINKS+=addr2ascii.3 ascii2addr.3
MLINKS+=byteorder.3 htonl.3 byteorder.3 htons.3 byteorder.3 ntohl.3 \
	byteorder.3 ntohs.3
MLINKS+=ethers.3 ether_aton.3 ethers.3 ether_hostton.3 ethers.3 ether_line.3 \
	ethers.3 ether_ntoa.3 ethers.3 ether_ntohost.3
MLINKS+=inet.3 addr.3 inet.3 inet_addr.3 inet.3 inet_aton.3 \
	inet.3 inet_lnaof.3 inet.3 inet_makeaddr.3 inet.3 inet_netof.3 \
	inet.3 inet_network.3 inet.3 inet_ntoa.3 \
	inet.3 inet_ntop.3 inet.3 inet_pton.3 \
	inet.3 network.3 inet.3 ntoa.3 
MLINKS+=inet_net.3 inet_net_ntop.3 inet_net.3 inet_net_pton.3
MLINKS+=linkaddr.3 link_addr.3 linkaddr.3 link_ntoa.3
MLINKS+=resolver.3 dn_comp.3 resolver.3 dn_expand.3 resolver.3 res_init.3 \
	resolver.3 res_mkquery.3 resolver.3 res_query.3 \
	resolver.3 res_search.3 resolver.3 res_send.3 resolver.3 dn_skipname.3 \
	resolver.3 ns_get16.3 resolver.3 ns_get32.3 \
	resolver.3 ns_put16.3 resolver.3 ns_put32.3
.endif
