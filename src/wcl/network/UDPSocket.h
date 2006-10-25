/**
 * Classes and defines for creating and maintaining networking
 *
 * $ClearChain: projects/clearchain/src/network/UDPSocket.h,v 1.4 2006/05/08 06:03:17 benjsc Exp $
 *
 * Copyright (C) 2005 Benjamin Close <Benjamin.Close@clearchain.com>
 */

#ifndef WCL_UDPSOCKET_H
#define WCL_UDPSOCKET_H

#include <string>
#include <wcl/network/Socket.h>
#include <wcl/network/UDPPacket.h>

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
