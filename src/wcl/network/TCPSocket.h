/**
 * Classes and defines for creating and maintaining networking
 *
 * $ClearChain: projects/clearchain/src/network/TCPSocket.h,v 1.4 2006/05/08 06:03:17 benjsc Exp $
 *
 * Copyright (C) 2005 Benjamin Close <Benjamin.Close@clearchain.com>
 */

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>

#include "Socket.h"

class TCPSocket: public Socket
{
	public:
		TCPSocket();
		TCPSocket ( const std::string, const unsigned port );
		void setFileDescriptor( int );
		void setRemoteAddress( sockaddr_in );

	protected: 
		virtual bool create();
};

#endif
