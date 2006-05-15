#include <stdio.h>
#include "../TCPServer.h"

int main(void)
{
    TCPServer s(55555);
    TCPSocket client;
    s.setBlockingMode( Socket::NONBLOCKING);
    
    while( true ){
	if ( s.accept( &client ) == false ){
	    printf("Server:Failed To accept, sleeping\n");
	    sleep(1);
	    continue;
	}
	printf("Server:Got a connection\n");
	client.write("You got mail!\n");
	client.close();
    }

    return 0;
}
