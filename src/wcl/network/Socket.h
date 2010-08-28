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

#ifdef WIN32
	#include <windows.h>
	#include <winsock.h>
	typedef int socklen_t;
#else					  /* Unix Includes */
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <signal.h>
	#include <stdio.h>
	#include <cstring>
#endif

namespace wcl {

/**
 * Classes and defines for creating and maintaining networking
 */
class Socket
{

	public:
		enum BlockingMode { BLOCKING, NONBLOCKING };

		virtual ~Socket();
		virtual ssize_t read ( void *buffer, size_t size );
		virtual ssize_t write( const void *buffer, size_t size );
		virtual ssize_t write( const std::string & );
		virtual void readUntil ( void *buffer, size_t size );
		virtual void writeUntil ( void *buffer, size_t size );
		virtual ssize_t getAvailableCount();
		virtual void close();
		virtual bool isValid() const;
		virtual bool setBlockingMode( const BlockingMode );
		virtual BlockingMode getBlockingMode() const;

		int operator *() const;
	
	protected:
		int sockfd;
		sockaddr_in address;
                BlockingMode blocking;

		Socket();
		virtual bool bind( const unsigned port);

};


class SocketException
{
public:
    SocketException(const Socket *);
    virtual ~SocketException();

    int getCause() const;
    const std::string getReason() const;

private:
    int sockid;
    int errornumber;
};


}; // namespace wcl

#endif
