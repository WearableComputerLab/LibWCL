#include <stdio.h>
#include "wcl/network/TCPSocket.h"

using namespace wcl;

int main(void)
{
	char buffer[4096];

	try{
		TCPSocket s("127.0.0.1", 55555);
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
