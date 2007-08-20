#include <stdio.h>
#include "../TCPSocket.h"

int main(void)
{
    char buffer[4096];

    TCPSocket s("localhost", 55555);
    s.read((char *)buffer, 4096);
    printf("Client: Server Said: %s\n", buffer);
    s.close();

    return 0;
}
