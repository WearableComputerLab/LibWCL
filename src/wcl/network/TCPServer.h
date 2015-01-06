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

#ifndef WCL_NETWORK_TCPSERVERSOCKET_H
#define WCL_NETWORK_TCPSERVERSOCKET_H

#include <wcl/api.h>
#include <wcl/network/TCPSocket.h>

namespace wcl {

#define  DEFAULT_SERVER_QUEUE_LENGTH 100

/**
 * This class represents a TCPServer.
 * You define the server as listening on a particular port,
 * Indicate if you want the server to be blocking or non blocking then
 * Call accept each time you wish to process a new connection.
 */
class WCL_API TCPServer: private TCPSocket
{
	public:
		TCPServer( const unsigned port, const unsigned queuelength = DEFAULT_SERVER_QUEUE_LENGTH );

		bool accept( TCPSocket *);

		/**
		 * These members are pulled into public scope. They
		 * are private initially due to private inheritance
		 */
		using TCPSocket::setBlockingMode;
		using TCPSocket::getBlockingMode;
		using TCPSocket::operator *;
};


}; // namespace wcl

#endif
