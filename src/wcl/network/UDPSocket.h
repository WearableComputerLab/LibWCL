/**
 * Classes and defines for creating and maintaining networking
 *
 * $ClearChain: projects/clearchain/src/network/UDPSocket.h,v 1.4 2006/05/08 06:03:17 benjsc Exp $
 *
 * Copyright (C) 2005 Benjamin Close <Benjamin.Close@clearchain.com>
 */

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <string>
#include "Socket.h"
#include "UDPPacket.h"

class UDPSocket: public Socket
{
    public:
	UDPSocket ( const std::string hostname, const unsigned port );
	int write(const void* buffer, int size);
	int read(void *buffer, int size);

	// UDP Packet interface
	int write( const UDPPacket * );
	int read( UDPPacket * );

    protected:
	UDPSocket();
	virtual bool create();

    private:
	sockaddr_in raddress;
};

#endif
