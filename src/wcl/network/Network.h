/**
 * Base Network Types
 *
 * $ClearChain: projects/clearchain/src/network/Network.h,v 1.2 2005/06/18 11:27:48 benjsc Exp $#
 *
 * Copyright (C) 2005 Benjamin Close <Benjamin.Close@clearchain.com>
 */

#ifndef WCL_NETWORK_H
#define WCL_NETWORK_H

#include <sys/types.h>
#include <net/ethernet.h>

typedef struct ether_addr EthernetAddress;
typedef struct in_addr IPAddress;

/**
 * The various header formats
 */

//
// Ethernet <net/ethernet.h> - Link Level
//
//#define ETHER_HDR_LEN           (ETHER_ADDR_LEN*2+ETHER_TYPE_LEN)
//#define	ETHER_VLAN_ENCAP_LEN	4	/* len of 802.1Q VLAN encapsulation */
//struct	ether_header {
//	u_char	ether_dhost[ETHER_ADDR_LEN];
//	u_char	ether_shost[ETHER_ADDR_LEN];
//	u_short	ether_type;
//};
//struct	ether_addr {
//	u_char octet[ETHER_ADDR_LEN];
//};
//#define ETHERTYPE_IP            0x0800  /* IP protocol */
//#define ETHERTYPE_ARP           0x0806  /* Addr. resolution protocol */
//#define ETHERTYPE_REVARP        0x8035  /* reverse Addr. resolution protocol */
//#define ETHERTYPE_VLAN          0x8100  /* IEEE 802.1Q VLAN tagging */
//
// ARP
//
//struct _arp_hdr
//{
//    u_short ar_hrd;                         /* format of hardware address */
//#define ARPHRD_ETHER     1                  /* ethernet hardware format */
//    u_short ar_pro;                         /* format of protocol address */
//    u_char  ar_hln;                         /* length of hardware address */
//    u_char  ar_pln;                         /* length of protocol addres */
//    u_short ar_op;                          /* operation type */
//#define ARPOP_REQUEST    1                  /* req to resolve address */
//#define ARPOP_REPLY      2                  /* resp to previous request */
//#define ARPOP_REVREQUEST 3                  /* req protocol address given hardwaa re */
//#define ARPOP_REVREPLY   4                  /* resp giving protocol address */
//#define ARPOP_INVREQUEST 8                  /* req to identify peer */
//#define ARPOP_INVREPLY   9                  /* resp identifying peer */
//    u_char ar_sha[6];                         /* sender hardware address */
//    u_char ar_spa[4];                         /* sender protocol address */
//    u_char ar_tha[6];                         /* target hardware address */
//    u_char ar_tpa[4];                         /* target protocol address */
//};
//

#endif
