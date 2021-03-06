/*
 First pass at a common ethernet link-up message.  Comments/bugs to guyton.
 
    kextname:	name of the driver or NULL if ok to use last word of the CFBundleIdentifier
    netif:	the network interface (used to get the bsd name)
    megabits:	10, 100 or 1000
    fullDuplex:	true if full duplex, else half
    flowState:	results of negotiating flow control (who's sending pause)
    port:	string that matches the label printed next to the physcial port or null
    loopback:	loopback mode string or null
    phyregs:	six debug phy registers.
 */

typedef enum {
    kLinkUpFlow_None	= 0,
    kLinkUpFlow_PauseTxOnly = 1,
    kLinkUpFlow_PauseRxOnly = 2,
    kLinkUpFlow_Symmetric = 3,
    /* as bits to construct above */
    kLinkFlow_Mask_TxPause= 0x1,
    kLinkFlow_Mask_RxPause= 0x2
} LinkUpFlow;

enum {				// recommended contents of phyregs, but driver & phy specific
    kLinkUp_Phy_OurStatus = 0,
    kLinkUp_Phy_ExtendedStatus,
    kLinkUp_Phy_OurAdvertised,
    kLinkUp_Phy_OurGigAdvertised,
    kLinkUp_Phy_PeerAdvertised,
    kLinkUp_Phy_PeerGigAdvertised,
    
    LinkUp_Phy_Count	// size of the array
};

extern kmod_info_t KMOD_INFO_NAME;	// generated by xcode for kexts

static inline void LinkUpMessage(
    char *buffer,			// or null to only do the IOLog
    int	  bufsize,			// size of the buffer
    const char *kextname,
    IONetworkInterface *netif,
    int	    megabits,
    bool    fullDuplex,
    LinkUpFlow	flowState,
    const char *port,
    UInt16 phyregs[LinkUp_Phy_Count],
    const char *loopback
)
{
    char line[256];
    
    if (buffer == NULL || bufsize == 0) {
	buffer = line;
	bufsize = sizeof(line);
    }
    if (kextname && *kextname == 0) kextname = NULL;
    if (port && *port == 0) port = NULL;
    if (loopback && *loopback == 0) loopback = NULL;
    
    // first find our kextname if null is passed in
    if (kextname == NULL) {
	char *period;
	//kextname = rindex(KMOD_INFO_NAME.name, '.');
	kextname = KMOD_INFO_NAME.name;
	while ((period = strchr(kextname, '.')) != NULL) {
	    kextname = ++period;
	}
	if (kextname == NULL || *kextname == 0)
	    kextname = "????";
    }
    
    snprintf(buffer, bufsize, "Ethernet [%s]: Link up on %s%d, %d-%s, %s-duplex, %s flow-control%s%s, Debug [%04x,%04x,%04x,%04x,%04x,%04x]%s%s",
	  kextname, netif->getNamePrefix(), netif->getUnitNumber(),
	  (megabits > 999) ? (int)(megabits / 1000) : megabits,
	  (megabits > 999) ? "Gigabit" : "Megabit",
	  (fullDuplex) ? "Full" : "Half",
	  (flowState == kLinkUpFlow_None) ? "No" :
	  ((flowState == kLinkUpFlow_Symmetric) ? "Symmetric" :
	   ((flowState == kLinkUpFlow_PauseTxOnly) ? "Symmetric Sending" :
	    ((flowState == kLinkUpFlow_PauseRxOnly) ? "Asymmetric Receiving" : "unknnown"))),
	  (port) ? ", Port " : "",
	  (port) ? port : "",
	  phyregs[0], phyregs[1], phyregs[2], phyregs[3], phyregs[4], phyregs[5],
    (loopback) ? ", Loopback " : "",
    (loopback) ? loopback : "");
    
    IOLog("%s\n", buffer);
}
    