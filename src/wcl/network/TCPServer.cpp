#include <assert.h>

#include "TCPServer.h"

#ifndef WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif

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
    if ( this->bind(port) == false ){
	throw new SocketException("Faile to bind to server socket");
    }
    
    if ( ::listen( this->sockfd, queuelength ) < 0 ){
	throw new SocketException("Failed ot set listen Queue");
    }
}

/**
 * Accept a connection on the setup socket. How the accept works depends on the blocking mode
 * of the socket. If the socket is in blocking mode, then accept will wait until a connection
 * is available. If the socket in non blocking mode, then accept will check to see if there is
 * a connection waiting, if there is not, it will return with status of false, else it 
 * will return with a status of true and modify the input socket.
 * 
 * @param socket A socket that can be modified to indicate the new client
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
