#include "TCPSocket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
	throw new SocketException("Unable to create Socket Object");
    } 

    // Setup the address to connect too
    address.sin_family = AF_INET;
    address.sin_port = htons ( port );

    // Attempt to perform a quick conversion, this only works provided the server string
    // passed in is a fully qualified name/ip. 
    if ( inet_pton( AF_INET, server.c_str(), &address.sin_addr ) == 0 ){

	// If we can't do a quick conversion, try the long way
	he = gethostbyname( server.c_str());
	if ( he == NULL ){
	    throw new SocketException("Unable to find specified host");
	}

	memcpy(&address.sin_addr, he->h_addr_list[0], he->h_length);
    }

    // Perform the connection
    if ( ::connect( this->sockfd, (sockaddr *)&address, sizeof(address)) == -1 ){
	this->sockfd = -1;
	throw new SocketException("Unable to connect to remote host, on specified port");
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

    // Enable the user to send to the broadcast address
    if (setsockopt (this->sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof (on))){
	this->close();
	return false;
    }

    return true; 
}


void TCPSocket::setFileDescriptor( int input)
{
    this->sockfd = input;
}

void TCPSocket::setRemoteAddress( sockaddr_in address)
{
    this->address = address;
}
