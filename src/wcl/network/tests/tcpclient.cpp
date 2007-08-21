#include <stdio.h>
#include "wcl/network/TCPSocket.h"

int main(void)
{
	char buffer[4096];

	try{
		TCPSocket s("10.220.99.84", 55555);
		s.read((char *)buffer, 4096);
		printf("Client: Server Said: %s\n", buffer);
		s.close();
	}
	catch( SocketException* e )
	{
		fprintf( stderr, e->getReason().c_str() );
	}

	return 0;
}
