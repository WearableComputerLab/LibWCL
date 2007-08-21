#include <stdio.h>
#include "wcl/network/UDPSocket.h"

int main(void)
{
    char buffer[4096];

    UDPSocket s("127.0.0.1", 55555);
    s.write("Hello", 5 );
    s.read((char *)buffer, 4096);
    printf("Client: Server Said: %s\n", buffer);
    s.close();

    return 0;
}
