#include <stdio.h>
#include "wcl/network/UDPServer.h"

int main(void)
{
    UDPServer s(55555);
    UDPPacket p(5);
    s.setBlockingMode( Socket::NONBLOCKING);
    
    while( true ){
	if ( s.read( &p ) > 0 ){
	    printf("Server:Got a packet\n");
	    s.write( &p );
	}
	printf("No packet received, sleeping\n");
	sleep(1);
    }

    return 0;
}
