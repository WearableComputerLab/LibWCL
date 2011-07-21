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

#include <string.h>

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Socket.h"
#include "SocketException.h"

namespace wcl {


/**
 * Constructor, create a socket which has not yet got a file descriptor 
 * allocated to it. This can only be called by subclasses to its the sub classes
 * responsibility to create the socket.
 */
Socket::Socket():
    sockfd(-1),blocking(BLOCKING)
{
    memset( &this->address, 0, sizeof( this->address ));

    /* Block the SIGPIPE signal to avoid it being sent when a read on a closed socket happens */
    sigset_t mask;

    /* Initialise the signal mask */
    sigemptyset(&mask);

    /* Add the new signal to the mask */
    sigaddset(&mask, SIGPIPE);

    /* Set the new signal to be blocked */
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

/**
 * Destructor, this closes the socket if its open
 */
Socket::~Socket()
{
    if ( this->isValid()){
	this->close();
    }
}

/**
 * Close the socket if it is open
 */
void Socket::close()
{
    if ( this->isValid()){
	::close( this->sockfd );
	this->sockfd = -1;
    }
}

/**
 * Indicate if the socket object is valid.  
 * This is a lightweight call that checks class internals only
 *
 * @return true if the socket object is valid, false if it is not
 */
bool Socket::isValid() const
{
    return (this->sockfd != -1);
}


/**
 * Bind the socket to a given port on any address of the local host
 * 
 * @param port The port to bind the socket to 
 * @return true if the bind was successful, false if socket was invalid or the bind failed
 */
bool Socket::bind( const unsigned port )
{
    if ( !this->isValid()){
	return false;
    }
	
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = htonl(INADDR_ANY);
    this->address.sin_port = htons ( port );

    if ( ::bind ( this->sockfd, (struct sockaddr *)&this->address, sizeof(this->address)) == -1 ){
	return false;
    }

    return true; 
}


/**
 * Attempt to read data from a socket into a buffer. This method
 * will read data and return as soon as it can. It will not block until
 * the given size has been read. If you want this functionality use readuntil.
 * If the remote socket is closed a SocketException is thrown.
 *
 * @param buffer The buffer to read into
 * @param size The size of the buffer
*  @param peek To peak at the message and not remove it from the network queue
 * @return The amount of data read if all ok, -1 on a read error, not caused
 *         by the close of a socket, Note, the amount
 *        of data actually read may not match what you requested. This function
 *        will return as soon as some data has been read. 0 will be returned
 *        if data is not available or would cause blocking to occur in
 *        NONBLOCKING mode
 * @throws SocketException if the remote end has closed the socket
 */
ssize_t Socket::read ( void *buffer, const size_t size, const bool peek) throw (SocketException)
{
    int flags = 0;

    if ( !isValid()){
	return -1;
    }

    if( peek )
	flags = MSG_PEEK;

    ssize_t retval = ::recv(this->sockfd, buffer, size, flags );
    if ( retval == -1 ){
	if (errno == ECONNRESET)
	    throw SocketException(this);

	// Non blocking will have errno set to
	// EAGAIN, we return 0 in this case
	if ( this->blocking == NONBLOCKING &&
	     (errno == EAGAIN || errno == EWOULDBLOCK )){
	    return 0;
	}
    } else if (retval == 0 && this->blocking == BLOCKING ){
	// The remote has gracefully closed the connection.
	throw SocketException(this);
    }
    return retval;
}

/**
 * Attempt to read data from a socket into a buffer. This method
 * will block until the requested size has been read. If you don't want
 * the socket to block, (even if it's nonblocking) use read. Please note
 * if using this on a non blocking socket, it will act like a busy loop thrashing.
 *
 * @param buffer The buffer to read into
 * @param size The amount of data to read, the buffer msut be at least this size
 * @throws SocketException if the remote end has forcable closed the socket
 */
void Socket::readUntil ( void *buffer, const size_t size ) throw (SocketException)
{
    ssize_t amount;
    ssize_t total = size;

    while( total > 0 ){
	amount = this->read( buffer, total );

	// We just can't increment a void * pointer as we don't know what it is
	// pointing too. However, we know that ::read returns in bytes so we 
	// cast to a type which can be validly incremented
	buffer = amount + (uint8_t *)buffer; 
	total-=amount;
    }
}


/**
 * Will attempt to write to a socket form a buffer. This method will block
 * until the requested size has been written. 
 *
 * @param buffer The buffer to read from.
 * @param size The amount of data to write, the buffer must be at least this size.
 * @throws SocketException if the remote peer has forced the socket closes.
 */
void Socket::writeUntil ( void *buffer, const size_t size ) throw (SocketException)
{
    ssize_t amount;
    ssize_t total = size;

    while( total > 0 ){
		amount = this->write( buffer, total );

		// We just can't increment a void * pointer as we don't know what it is
		// pointing too. However, we know that ::read returns in bytes so we 
		// cast to a type which can be validly incremented
		buffer = amount + (uint8_t *)buffer; 
		total-=amount;
    }
}


/**
 * Attempt to write size characters from buffer to the socket.
 * This method throws an exception if the remote end has forcable closed
 * the socket.
 *
 * @param buffer The buffer containing the data to write
 * @param size The amount of characters to write, the buffer must be this big
 * @return The amount of charaters written
 * @throws SockcetException if the remote peer has forced the socket * closed
 */
ssize_t Socket::write( const void *buffer, const size_t size ) throw (SocketException)
{
	if ( !isValid()){
		return -1;
	}

	ssize_t retval =  ::write( this->sockfd, buffer, size );
	if ( retval == -1 ){
		throw SocketException(this);
	}
	return retval;
}

/**
 * Write the given string to the socket. 
 *
 * @param string The string to write
 * @return -1 if the socket is invalid, or the number of bytes written otherwise 
 * @throws SocketException if the write fails.
 */ 
ssize_t Socket::write( const std::string &string ) throw (SocketException)
{
    return this->write( string.c_str(), string.size());
}

/**
 * Set the blocking mode of this socket.
 * The blocking mode of a socket indicates whether read / accept / write calls will
 * block. Win32 does not support non blocking sockets without a HWND hence this library doesnt
 * 
 * @param mode The mode to set this socket too
 * @return true if the call was successful, false otherwise
 */
bool Socket::setBlockingMode( const BlockingMode mode)
{
    int flags;

    if ( !isValid()){
	return false;
    }

    // In order to set the blocking flag, we must be careful not 
    // to stop on any other flags that have been set. Hence
    // we obtain all the flags and adjust the blocking flag only
    flags = ::fcntl(this->sockfd, F_GETFL, 0 /* ALLFLAGS */);

    switch( mode ) {
	case BLOCKING:
	    flags &=~O_NONBLOCK;
	    if ( ::fcntl( this->sockfd, F_SETFL, flags ) == 0 ){
                this->blocking = mode;
		return true;
	    } 
	    break;
	case NONBLOCKING:
	    flags |= O_NONBLOCK;
	    if ( ::fcntl( this->sockfd, F_SETFL, flags ) == 0 ){
                this->blocking = mode;
		return true;
	    } 
	    break;
    }

    return false;
}

/**
 * Obtain whether the socket is set to blocking or non blocking mode.
 * Note: A windows socket will always be blocking
 *
 * @return BLOCKING or NONBLOCKING depending on the socket state, if the socket has not been setup
 *         then it is in the default state of BLOCKING.
 */
Socket::BlockingMode Socket::getBlockingMode() const 
{
    return this->blocking;
}

/**
 * Return the file descriptor associated with this socket
 *
 * @return the socket descriptor, -1 if its in error
 */
int Socket::operator *() const
{
    return this->sockfd;
}

/**
 * Obtain the amount of bytes currently available for reading
 *
 * @return The amount of bytes available or 0 if the socket is closed
 * @throws SocketException if the remote has closed the socket
 */
ssize_t Socket::getAvailableCount()
{
    int bytesAvailable;
    int status = ioctl (this->sockfd, FIONREAD, &bytesAvailable);
    if ( status == -1 && errno == ECONNRESET){
	throw SocketException(this);
    }
    return bytesAvailable;
}

sockaddr_in Socket::resolve( const char *input, const unsigned port) throw (SocketException)
{
    struct hostent *he;
    struct sockaddr_in address;

    memset(&address, 0x0, sizeof(struct sockaddr_in));

    // Setup the address to connect too
    address.sin_family = AF_INET;
    address.sin_port = htons ( port );

    // Attempt to perform a quick conversion, this only works provided the server string
    // passed in is a fully qualified name/ip.
    if ( inet_pton( AF_INET, input, &address.sin_addr ) == 0 ){

	// If we can't do a quick conversion, try the long way
	he = gethostbyname( input );
	if ( he == NULL ){
	    throw SocketException(NULL);
	}

	memcpy(&address.sin_addr, he->h_addr_list[0], he->h_length);
    }

    return address;
}

void Socket::storeResolve( const char *input, const unsigned port) throw (SocketException)
{
    struct sockaddr_in temp = Socket::resolve( input, port );
    memcpy( &this->address, &temp, sizeof(struct sockaddr_in));
}


} // namespace wcl
