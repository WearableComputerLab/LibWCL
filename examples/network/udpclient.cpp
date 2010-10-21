#include <stdio.h>
#include <iostream>
#include "wcl/network/UDPSocket.h"

using namespace wcl;
using namespace std;

int main(void)
{
    char buffer[4096];

    try {
	UDPSocket s("127.0.0.1", 55555);
	s.write("Hello", 5 );
	s.read((char *)buffer, 4096);
	printf("Client: Server Said: %s\n", buffer);
	s.close();
    } catch (SocketException e)
    {
	cout << e.what() << endl;
    }

    return 0;
}
