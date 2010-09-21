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
#include <assert.h>

#include "TCPServer.h"

namespace wcl {

/**
 * Construct a TCP server to listen on the given port of the localhost.
 * The server will queue at most queuelength connections
 *
 * @param port The port the server should listen on
 * @param queuelength The length of the listen queue
 * @throws SockExecption if the server port is in user or the listen queue is too long
 */
TCPServer::TCPServer( const unsigned port, const unsigned queuelength )
{
    if ( this->create() == false ){
	throw new SocketException(this);
    }

    if ( this->bind(port) == false ){
	throw new SocketException(this);
    }

    if ( ::listen( this->sockfd, queuelength ) < 0 ){
	throw new SocketException(this);
    }
}

/**
 * Accept a connection on the setup socket. How the accept works depends on the blocking mode
 * of the server. If the server is in blocking mode, then accept will wait until a connection
 * is available. If the server in non blocking mode, then accept will check to see if there is
 * a connection waiting, if there is not, it will return with status of false, else it
 * will return with a status of true and modify the input socket.
 *
 * @param socket A socket that can be modified to indicate the new client. If the socket is already valid, it will first be closed
 * @return true on a successful client connection, false if in non blocking mode and no clients waiting
 */
bool TCPServer::accept( TCPSocket *socket )
{
    assert( socket != NULL );

    int addr_length = sizeof(this->address);
    int newfd  = ::accept( this->sockfd, (sockaddr *)&this->address, (socklen_t*)&addr_length);

    // Close the existing socket if it allocated
    if( socket->isValid()){
	socket->close();
    }

    // Set the content of the new socket
    socket->setFileDescriptor( newfd );
    socket->setRemoteAddress( this->address );

    return socket->isValid();
}


}; // namespace wcl
