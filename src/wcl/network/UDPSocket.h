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

#ifndef WCL_NETWORK_UDPSOCKET_H
#define WCL_NETWORK_UDPSOCKET_H

#include <string>
#include <wcl/api.h>
#include <wcl/network/Socket.h>
#include <wcl/network/UDPPacket.h>

namespace wcl {

/**
 * A UDPSocket creates the perception of an end to end connection
 * like a TCP connection. 
 */
class WCL_API UDPSocket: public Socket
{
    public:
	UDPSocket ( const std::string hostname, const unsigned port );
	ssize_t write(const void* buffer, size_t size);
	ssize_t read(void *buffer, size_t size);

	// UDP Packet interface
	ssize_t write( const UDPPacket * );
	ssize_t read( UDPPacket * );

    protected:
	UDPSocket();
	virtual bool create();

    private:
	sockaddr_in raddress;
};


}; // namespace wcl

#endif
