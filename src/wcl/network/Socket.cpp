
#include <string.h>

#ifdef WIN32
	#include <strstream>
#else /* UNIX */
	#include <iostream>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <errno.h>
#endif

#include "Socket.h"


/**
 * For Win32 winsock needs to be initialised before
 * we can use the subsystem. This is done in the first constructor call to
 * Socket. But we need to know if it's been done or not
 */
#ifdef WIN32
static bool win32init = false;
static WORD sockVersion = MAKEWORD(1,1); // We'd like Winsock version 1.1
static WSADATA wsaData;
#endif


/**
 * Constructor, create a socket which has not yet got a file descriptor 
 * allocated to it. This can only be called by subclasses to its the sub classes
 * responsibility to create the socket.
 */
Socket::Socket()
{
    this->sockfd = -1;
    memset( &this->address, 0, sizeof( this->address ));

#ifdef WIN32
	if(!win32init){
		WSAStartup(sockVersion, &wsaData);
		win32init=true;
	}
#endif

#ifdef __linux
	/* Block the SIGPIPE signal to avoid it being sent when a read on a closed socket happens */
	sigset_t mask;

	/* Initialise the signal mask */
	sigemptyset(&mask);

	/* Add the new signal to the mask */
	sigaddset(&mask, SIGPIPE);

	/* Set the new signal to be blocked */
	sigprocmask(SIG_BLOCK, &mask, NULL);
#endif


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
#ifdef WIN32
	::closesocket( this->sockfd );
#else
	::close( this->sockfd );
#endif
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
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons ( port );

    if ( ::bind ( this->sockfd, (struct sockaddr *)&this->address, sizeof(this->address)) == -1 ){
	return false;
    }

    return true; 
}


/**
 * Attempt to read data from a socket into a buffer.
 * If the remote socket is closed a SocketException is
 * thrown.
 *
 * @param buffer The buffer to read into
 * @param size How much to read, the buffer must be at least this size
 * @return The amount of data read if all ok, -1 on a read error, not caused
 *         by the close of a socket, 0 on a graceful close
 * @throws SocketException if the remote end has forcable closed the * socket 
 */
int Socket::read ( void *buffer, size_t size )
{

    if ( !isValid()){
	return -1;
    }

#ifdef WIN32
    int retval = ::recv(this->sockfd, (char*)buffer, size, 0x0);
    if( retval == SOCKET_ERROR ){
	throw new SocketException(this);	
    }
    return retval;
#else
    int retval = ::read(this->sockfd, buffer, size );
    if ( retval == -1 ){
	throw new SocketException(this);
    }
    return retval;
#endif
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
int Socket::write( const void *buffer, size_t size )
{
    if ( !isValid()){
	return -1;
    }

#ifdef WIN32
    int retval = ::send(this->sockfd, (const char *)buffer, size, 0x0);
    if ( retval == SOCKET_ERROR ){
	throw new SocketException(this);
    }
    return retval;
#else
    int retval =  ::write( this->sockfd, buffer, size );
    if ( retval == -1 ){
	throw new SocketException(this);
    }
    return retval;
#endif
}

/**
 * Write the given string to the socket. 
 *
 * @param string The string to write
 * @return -1 on failure, or the number of bytes written otherwise 
 */ 
int Socket::write( const std::string &string )
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

#ifdef WIN32
	/** 
         * Windows does non blocking very, very differently.
	 * hence we don't support it. We return false to indicate this.
	 */
	return false;

#else /* UNIX */
    // In order to set the blocking flag, we must be careful not 
    // to stop on any other flags that have been set. Hence
    // we obtain all the flags and adjust the blocking flag only
    flags = ::fcntl(this->sockfd, F_GETFL, 0 /* ALLFLAGS */);

    switch( mode ) {
	case BLOCKING:
	    flags &=~O_NONBLOCK;
	    if ( ::fcntl( this->sockfd, F_SETFL, flags ) == 0 ){
		return true;
	    } 
	    break;
	case NONBLOCKING:
	    flags |= O_NONBLOCK;
	    if ( ::fcntl( this->sockfd, F_SETFL, flags ) == 0 ){
		return true;
	    } 
	    break;
    }

    return false;
#endif
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
#ifdef WIN32
	return BLOCKING;

#else /* UNIX */
    if ( !isValid() || ::fcntl( this->sockfd, F_GETFL, O_NONBLOCK) == 0 ){
		return BLOCKING;
    }
    return NONBLOCKING;
#endif
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



/*==============================================================================
 *
 *  SocketException Class Methods
 *
 *=============================================================================*/

/**
 * Construct a socket exception with the specified reason
 *
 * @param reason The reason the exception occurred
 */
SocketException::SocketException( const Socket *s )
{
    this->sockid = **s;

#ifdef WIN32
    this->errornumber  = WSAGetLastError();
#else
    this->errornumber  = errno; /* errno defined in errno.h */
#endif
}

SocketException::~SocketException()
{}

/**
 * Obtain the reason why the socket exception was
 * thrown
 * 
 * @return The reason this socket exception occurred
 */
const std::string SocketException::getReason() const
{
#ifdef WIN32
    std::strstream s;
    s << "Winsock Errno:";
    s << this->errornumber;
    s << std::ends;
    return s.str();
#else
    return strerror( this->errornumber );
#endif
}

