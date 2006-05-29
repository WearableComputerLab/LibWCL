#include <assert.h>
#include "UDPSocket.h"

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
UDPSocket::UDPSocket ( const std::string server, const unsigned port )
{
    struct hostent *he;

#ifdef WIN32
	// If we can't do a quick conversion, try the long way
	he = gethostbyname( server.c_str());
	if ( he == NULL ){
	    throw new SocketException("Unable to find specified host");
	}

	memcpy(&raddress.sin_addr, he->h_addr_list[0], he->h_length);
#else

    // Lookup the address of the remote machine
    // Attempt to perform a quick conversion, this only works provided the server string
    // passed in is a fully qualified name/ip. 
    if ( inet_pton( AF_INET, server.c_str(), &raddress.sin_addr ) == 0 ){

	// If we can't do a quick conversion, try the long way
	he = gethostbyname( server.c_str());
	if ( he == NULL ){
	    throw new SocketException("Unable to find specified host");
	}

	memcpy(&raddress.sin_addr, he->h_addr_list[0], he->h_length);
    }
#endif
    // Store the port add set the address type
    raddress.sin_family = AF_INET;
    raddress.sin_port=htons(port);


    if ( this->create() == false ){
	throw new SocketException( "Failed to open socket");
    }

    // Bind to a port on the local host, first free port.
    if ( this->bind(0) == false ){
	this->close();
	throw new SocketException("Failed to bind to port");
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

int UDPSocket::read(void *buffer, int size)
{
#ifdef WIN32
    return recv(this->sockfd, (char *)buffer, size, 0x0);
#else 
    return recv(this->sockfd, buffer, size, 0x0);
#endif
}
	
int UDPSocket::write(const void *buffer, int size)
{
#ifdef WIN32
    return sendto(this->sockfd, (const char *)buffer, size, 0x0, (struct sockaddr *)&raddress, sizeof(raddress));
#else
    return sendto(this->sockfd, buffer, size, 0x0, (struct sockaddr *)&raddress, sizeof(raddress));
#endif
}

/**
 * Write the given UDPPacket to the socket. The Recipient field of the
 * UDPPacket is used to indicate where to send this packet to.
 *
 * @param The packet to send to the remote host
 * @return -1 on error, or the amount of data written
 */
int UDPSocket::write( const UDPPacket *packet )
{
    assert ( packet != NULL && packet->getData() != NULL  );

    raddress = packet->getRecipient();

#ifdef WIN32
    return sendto( this->sockfd, 
		    (const char *)packet->getData(), 
		    packet->getSize(), 
		    0x0, 
		    (struct sockaddr *)&raddress, 
		    sizeof(raddress));
#else
    return sendto( this->sockfd, 
		    packet->getData(), 
		    packet->getSize(), 
		    0x0, 
		    (struct sockaddr *)&raddress, 
		    sizeof(raddress));
#endif
}

/**
 * Read data into the given packet. The packet should be preallocated with a valid buffer & size
 *
 * @param packet the Packet to read into
 * @return -1 on error, 0 on read with nothing waiting, or the amount read
 */
int UDPSocket::read( UDPPacket *packet )
{
    assert( packet != NULL && packet->getData() != NULL);

#ifdef WIN32
    struct sockaddr_in clientAddress;
    int clientAddressLen = sizeof(clientAddress);
    int result =  recvfrom( this->sockfd, 
			(char *)packet->getData(), 
			packet->getSize(), 
			0x0,
			(struct sockaddr *)&clientAddress, 
			(socklen_t*)&clientAddressLen);
#else /* UNIX */
    int result =  recvfrom( this->sockfd, 
			packet->getData(), 
			packet->getSize(), 
			0x0,
			(struct sockaddr *)&clientAddress, 
			(socklen_t*)&clientAddressLen);
#endif

    if( result >= -1 )packet->setRecipient( clientAddress );
    return result;
}

