#include "UDPServer.h"

#ifndef WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif

/** 
 * Create a UDP Server that listens on the given port
 *
 * @param port The port to listen on
 * @throws SocketException If a socket or the binding to the port fails
 */
UDPServer::UDPServer( const unsigned port )
{
    if ( this->create() == false ){
	throw new SocketException(this);	
    }

    if ( this->bind(port) == false ){
	this->close();
	throw new SocketException(this);
    }
}
