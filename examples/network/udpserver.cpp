#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "wcl/network/UDPServer.h"

using namespace wcl;
using namespace std;

int main(void)
{
	UDPServer s(55555);
	UDPPacket p(5);
	s.setBlockingMode(Socket::NONBLOCKING);

	while( true ){
	    try {
		if ( s.read( &p ) > 0 ){
		    printf("Server:Got a packet\n");
		    s.write( &p );
		}
	    }

	    catch (SocketException e )
	    {
		cout << e.what() << endl;
	    }
	    printf("No packet received, sleeping\n");
	    sleep(1);
	}
	return 0;
}
