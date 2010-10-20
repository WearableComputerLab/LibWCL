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

#ifndef WCL_NETWORK_SOCKET_H
#define WCL_NETWORK_SOCKET_H

#include <string>

	#include <sys/types.h>
	#include <netinet/in.h>
	#include <signal.h>
	#include <stdio.h>
	#include <cstring>

#include <wcl/api.h>
#include <wcl/network/SocketException.h>

namespace wcl {

/**
 * The Socket class is the base class for all networking transactions.
 * It provides default operations for tcp/udp and the ability to get
 * the associated file descriptor via *operator()
 */
class WCL_API Socket
{

	public:
		enum BlockingMode { BLOCKING, NONBLOCKING };

		virtual ~Socket();
		virtual ssize_t read ( void *buffer, size_t size ) throw (SocketException);
		virtual ssize_t write( const void *buffer, size_t size ) throw (SocketException);
		virtual ssize_t write( const std::string & ) throw (SocketException);
		virtual void readUntil ( void *buffer, size_t size ) throw (SocketException);
		virtual void writeUntil ( void *buffer, size_t size ) throw (SocketException);
		virtual ssize_t getAvailableCount();
		virtual void close();
		virtual bool isValid() const;
		virtual bool setBlockingMode( const BlockingMode );
		virtual BlockingMode getBlockingMode() const;

		int operator *() const;

		/**
		 * Given a ip address, or host name resolve the input to a
		 * sockaddr_in structure. Note this call may appear to hang if a
		 * dns query is required to resolve the hostname - at least
		 * until dns timesout
		 *
		 * @param input The ip address or hostname to convert
		 * @param port The port to put in the struct
		 * @return A valid sockaddr_in structure
		 * @throw SocketException upon resolution failure
		 */
		static sockaddr_in resolve(const char *input, const unsigned port) throw (SocketException);
	
	protected:
		int sockfd;
		struct sockaddr_in address;
                BlockingMode blocking;

		Socket();

		/**
		 * Resolve the given ip/hostname and store it in the 'address' field
		 * @param input ip/hostname to convert
		 * @param port The port to store in the strcut
		 * @throw SocketException upon resolving issue
		 */
		void storeResolve( const char *input, const unsigned port) throw (SocketException);
		virtual bool bind( const unsigned port);

};

}; // namespace wcl

#endif
