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
#include "TCPSocket.h"


#ifndef WIN32 /* UNIX */
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

namespace wcl {

/**
 * Default Constructor, this constructor should only be used by
 * clients wanting to use a TCPServer. Hence no initialisation is
 * required. The socket should by default be in an invalid state.
 */
TCPSocket::TCPSocket()
{}

/**
 * Create a TCPSocket and attempt to connect the socket to the specified
 * server on the given port. The first available local port is used for the
 * connection.
 *
 * @param server The server to connect to, this can be a string based ip, or hostname
 * @param port The remote port to connect to
 * @throws SocketError If there was a problem resolving the hostname or connecting to the host
 */
TCPSocket::TCPSocket( const std::string server, const unsigned port )
{
    struct hostent *he;

    // Create a new socket
    if ( !this->create()){
	throw new SocketException(this);
    } 

    // Setup the address to connect too
    address.sin_family = AF_INET;
    address.sin_port = htons ( port );

#ifdef WIN32

	// If we can't do a quick conversion, try the long way
	he = gethostbyname( server.c_str());
	if ( he == NULL ){
	    throw new SocketException(this);
	}

	memcpy(&address.sin_addr, he->h_addr_list[0], he->h_length);


#else /* Unix */
    // Attempt to perform a quick conversion, this only works provided the server string
    // passed in is a fully qualified name/ip. 
    if ( inet_pton( AF_INET, server.c_str(), &address.sin_addr ) == 0 ){

	// If we can't do a quick conversion, try the long way
	he = gethostbyname( server.c_str());
	if ( he == NULL ){
	    throw new SocketException(this);
	}

	memcpy(&address.sin_addr, he->h_addr_list[0], he->h_length);

    }
#endif

    // Perform the connection
    if ( ::connect( this->sockfd, (sockaddr *)&address, sizeof(address)) == -1 ){
	this->close();
	throw new SocketException(this);
    }
}

/**
 * Create a socket of type SOCK_STREAM and allow more than one socket to listen on 
 * the one port if required
 *
 * @return true if The socket was created successfully, else false
 */
bool TCPSocket::create()
{
    this->sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    if ( !this->isValid()){
	return false;
    }

    // Allow multiple sockets to listen on the one port for the application
    int on =1;
    if ( setsockopt ( this->sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) == -1 ){
	this->close();
	return false;
    }

#ifndef WIN32 /* UNIX */
    // XXX/FIXME: For some reason this fails under MSVC
    // with error 10042 - Unknown, invalid option (SO_BROADCAST)
    // I don't know why at present - Ben 20060529

    // Enable the user to send to the broadcast address
    on = 1;
    if (setsockopt (this->sockfd, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof (on)) == -1){
	this->close();
	return false;
    }

#endif

    return true; 
}


/**
 * Update the file descriptor that the TCPSocket uses. This call will also
 * update the blocking state of the descriptor to what the TCPSocket blocking
 * state currently is set to.
 *
 * @param input The new file descriptor to use as a TCPSocket
 */
void TCPSocket::setFileDescriptor( const int input)
{
    this->sockfd = input;

    // We must update the blocking state of the input descriptor else
    this->setBlockingMode( this->blocking );
}

/**
 * Set the remote address represented by this socket
 *
 * @param address The new address this socket should represent
 */
void TCPSocket::setRemoteAddress( const sockaddr_in address)
{
    this->address = address;
}


}; // namespace wcl
