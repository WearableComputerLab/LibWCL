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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "UDPServer.h"

namespace wcl {

/**
 * Create a UDP Server that listens on the given port
 *
 * @param port The port to listen on
 * @param mcastgroup The multicast group to use (if required)
 * @throws SocketException If a socket or the binding to the port fails
 */
UDPServer::UDPServer( const unsigned port, const std::string &mcastgroup ) throw (SocketException):
    hasmcast(false)
{

    if ( this->create() == false ){
	throw SocketException(this);
    }

    if ( this->bind(port) == false ){
	this->close();
	throw SocketException(this);
    }

    if( mcastgroup.size() ){
	hasmcast=true;

	// Store the local lookup of the address
	this->storeResolve(mcastgroup.c_str(), port);

	// Ask the kernel to enable multicast support for the requested group
	
	this->mreq.imr_multiaddr.s_addr=this->address.sin_addr.s_addr;
	this->mreq.imr_interface.s_addr=INADDR_ANY;
	if (setsockopt(this->sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) == -1) {
	    this->close();
	    throw SocketException(this);
	}
    }
}

}; // namespace wcl
