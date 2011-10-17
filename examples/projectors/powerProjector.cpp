#include <stdio.h>
#include "wcl/network/TCPServer.h"

using namespace wcl;

int main(void)
{
    char buffer[256];

    try {
        TCPSocket projector("127.0.0.1",7134);
       
        // We want to write: 02H 00H 00H 00H 00H 00H 02H
        // to the projector. Converting this to decimal. 
        char buffer[] = { (char)2, 0, 0, 0, 0, (char)2 }; 
        projector.write((char*) buffer, 6);
        
        projector.read((char *)buffer, 256);
        printf("Projector responded: %s\n", buffer);
        projector.close();

    } catch (SocketException* e) {
        fprintf( stderr, "%s", e->what() );
    }

    return 0;
}
