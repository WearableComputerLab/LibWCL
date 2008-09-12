/*-
 * Copyright (c) 2005-2008 Benjamin Close <Benjamin.Close@clearchain.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef WCL_NETWORK_H
#define WCL_NETWORK_H

#include <sys/types.h>
#include <net/ethernet.h>

/**
 * Base Network Types
 */

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
