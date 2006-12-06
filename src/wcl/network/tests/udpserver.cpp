#include <stdio.h>
#include "../UDPServer.h"

int main(void)
{
try{
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
}
    catch( SocketException *e )
	{
		printf("%s\n", e->getReason().c_str());
	}

    return 0;
}
