/**
 * Classes and defines for creating and maintaining networking
 *
 * $ClearChain: projects/clearchain/src/network/UDPServer.h,v 1.1 2006/05/08 06:03:17 benjsc Exp $
 *
 * Copyright (C) 2005 Benjamin Close <Benjamin.Close@clearchain.com>
 */

#ifndef WCL_UDPSERVER_H
#define WCL_UDPSERVER_H

#include <wcl/network/UDPSocket.h>
#include <wcl/network/UDPPacket.h>

class UDPServer: public UDPSocket
{
    public:
	UDPServer( const unsigned port );
};

#endif
