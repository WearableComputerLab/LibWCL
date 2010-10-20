/*-
 * Copyright (c) 2006-2008 Benjamin Close <Benjamin.Close@clearchain.com>
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
#ifndef WCL_NETWORK_UDPSERVER_H
#define WCL_NETWORK_UDPSERVER_H

#include <string>
#include <wcl/api.h>
#include <wcl/network/UDPSocket.h>
#include <wcl/network/UDPPacket.h>
#include <arpa/inet.h>

namespace wcl {

/**
 * The UDPServer class can be used to listen on a port and 
 * and receive UDPPackets.
 */
class WCL_API UDPServer: public UDPSocket
{
    public:
	UDPServer( const unsigned port, const std::string &mcastgroup = "" ) throw (SocketException);

	/**
	 * Write the given UDPPacket to the socket. The Recipient field of the
	 * UDPPacket is used to indicate where to send this packet to. If the field
         * is not set the server will set the field to the multicast group if multicast
	 * is enabled. Else pass it down to UDPSocket to process
	 *
	 * @param The packet to send to the remote host
	 * @return -1 on error, or the amount of data written
	 * @throw Socket Exception on a failed write
	 */
	ssize_t UDPSocket::write;

    private:
	bool hasmcast;
	struct ip_mreq mreq;

};


}; // namespace wcl

#endif
