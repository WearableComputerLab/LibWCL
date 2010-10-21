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
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <wcl/network/UDPSocket.h>

namespace wcl {

/**
 * Called by subclasses. No initialisation is done
 * as we don't know the intent of the subclass.
 */ 
UDPSocket::UDPSocket()
{}

/**
 * Create a UDPSocket which will send data to the given host/port.
 * The socket is automatically setup to communicate with the remote host for 
 * all non UDPPacket based interactions.
 *
 * @param server The name of the host to send to
 * @param port The port on the remote host
 * @throws SocketException if the remote host could not be found
 */
UDPSocket::UDPSocket ( const std::string &server, const unsigned port ) throw (SocketException)
{
    struct hostent *he;

    this->storeResolve( server.c_str(), port );

    if ( this->create() == false ){
	throw SocketException(this);
    }

    // Bind to a port on the local host, first free port.
    if ( this->bind(0) == false ){
	this->close();
	throw SocketException(this);
    }
}

/**
 * Create a UDP Socket
 *
 * @return false if the socket could not be created, true otherwise
 */
bool UDPSocket::create()
{
    this->sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

    if ( !this->isValid()){
	return false;
    }

    // Allow multiple sockets to listen on the one port for the application
    int on =1;
    if ( setsockopt ( this->sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) == -1 ){
	this->close();
	return false;
    }

    // Enable the user to send to the broadcast address
    if (setsockopt (this->sockfd, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof (on))){
	this->close();
	return false;
    }

    return true; 
}

ssize_t UDPSocket::read(void *buffer, size_t size) throw (SocketException)
{
    ssize_t retval = recv(this->sockfd, buffer, size, 0x0);
    if ( retval == -1 ){
	throw SocketException(this);
    }
    return retval;
}
	
ssize_t UDPSocket::write(const void *buffer, size_t size) throw (SocketException)
{
    ssize_t retval = sendto(this->sockfd, buffer, size, 0x0, (struct sockaddr *)&address, sizeof(address));

    if (retval == -1 ){
	throw SocketException(this);
    }
    return retval;
}

/**
 * Write the given UDPPacket to the socket. The Recipient field of the
 * UDPPacket is used to indicate where to send this packet to. If the
 * field is not set the packet will be sent to the end point of
 * the socket connection
 *
 * @param The packet to send to the remote host
 * @return -1 on error, or the amount of data written
 * @throw SocketException on write failure
 */
ssize_t UDPSocket::write( const UDPPacket *packet ) throw (SocketException)
{
    assert ( packet != NULL && packet->getData() != NULL  );

    struct sockaddr_in raddress;

    if( packet->hasAddress())
	raddress = packet->getRecipient();
    else 
	raddress = this->address;

    ssize_t retval = sendto( this->sockfd, 
		    packet->getData(), 
		    packet->getSize(), 
		    0x0, 
		    (struct sockaddr *)&raddress, 
		    sizeof(raddress));
    if ( retval == -1 ){
	throw SocketException(this);
    }
    return retval;
}

/**
 * Read data into the given packet. The packet should be preallocated with a valid buffer & size
 *
 * @param packet the Packet to read into
 * @return -1 on error, 0 on read with nothing waiting, or the amount read
 */
ssize_t UDPSocket::read( UDPPacket *packet ) throw (SocketException)
{
    assert( packet != NULL && packet->getData() != NULL);

    struct sockaddr_in clientAddress;
    size_t clientAddressLen = sizeof(clientAddress);

    ssize_t result =  recvfrom( this->sockfd, 
			packet->getData(), 
			packet->getSize(), 
			0x0,
			(struct sockaddr *)&clientAddress, 
			(socklen_t*)&clientAddressLen);

    if ( result == -1 ){
	throw SocketException(this);
    }

    packet->setRecipient( clientAddress );
    return result;
}


}; // namespace wcl
