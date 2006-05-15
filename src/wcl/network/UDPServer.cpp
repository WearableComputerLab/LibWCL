#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "UDPServer.h"

/** 
 * Create a UDP Server that listens on the given port
 *
 * @param port The port to listen on
 * @throws SocketException If a socket or the binding to the port fails
 */
UDPServer::UDPServer( const unsigned port )
{
    if ( this->create() == false ){
	throw new SocketException("Unable To Create Server Socket");	
    }

    if ( this->bind(port) == false ){
	this->close();
	throw new SocketException("Unable to bind server socket to port");
    }
}
