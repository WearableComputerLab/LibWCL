/**
 * Classes and defines for creating and maintaining networking
 *
 * $ClearChain: projects/clearchain/src/network/TCPServer.h,v 1.1 2006/05/08 06:03:16 benjsc Exp $
 *
 * Copyright (C) 2005 Benjamin Close <Benjamin.Close@clearchain.com>
 */

#ifndef TCPSERVERSOCKET_H
#define TCPSERVERSOCKET_H

#include <wcl/network/TCPSocket.h>

#define  DEFAULT_SERVER_QUEUE_LENGTH 100

/**
 * This class represents a TCPServer. 
 * You define the server as listening on a particular port, 
 * Indicate if you want the server to be blocking or non blocking then
 * Call accept each time you wish to process a new connection.
 */
class TCPServer: private TCPSocket
{
	public:
		TCPServer( const unsigned port, const unsigned queuelength = DEFAULT_SERVER_QUEUE_LENGTH );

		bool accept( TCPSocket *);

		TCPSocket::setBlockingMode;
		TCPSocket::getBlockingMode;
		TCPSocket::operator *;
};

#endif
