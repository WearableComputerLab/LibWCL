#include <cstdlib>
#include <string>
#include <iostream>
#include <stdio.h>
#include "wcl/network/TCPServer.h"

using namespace wcl;

void usage() {
    printf("NAME\n\tpcontrol - Network command controller for NEC projectors\n\n"
            "SYNOPSIS\n\t pcontrol [options] -a <address> -p <port>\n\n"
            "When no options, address or port are specified, the defaults of:\n"
            "127.0.0.1:7142\n"
            "are used.\n\n"
            "Options:\n"
            "\t-h\n\t--help\n"
            "\t\tDisplay usage options.\n"
            "\t-a\n\t--address\n"
            "\t\tThe address of the projector to communicate with.\n"
            "\t-p\n\t--port\n"
            "\t\tThe port to communicate on.\n"
    );
}

int main(int argc, char** argv) {

    std::string address = "";    
    unsigned int port = 0;
    if ( argc >= 2 ) {

        for ( int counter = 0; counter <= argc ; ++counter ) {

            if ( strcmp ( argv[counter], "--help" ) == 0 || strcmp ( argv[counter], "-h" ) == 0 ) {
                usage();
                return 1;
            }

            if ( strcmp ( argv[counter], "--address" ) == 0 || strcmp (argv[counter], "-a" ) == 0 ) {
                if ( counter+1 <= argc ) {
                    //TODO double check that argc[counter+1] is a valid IP address.
                    address=argv[counter+1];
                    counter++;
                } else {
                    usage();
                    return 1;
                }   
            }

            if ( strcmp ( argv[counter], "--port" ) == 0 || strcmp (argv[counter], "-p" ) == 0 ) {
                if ( counter+1 <= argc ) {
                    char *pEnd;
                    port = strtol ( argv[counter+1], &pEnd, 10 );
                    counter++;
                } else {
                    usage();
                    return 1;
                }   
            }
        }
   } else {
       usage();
       return -1;
    }
    
    
    unsigned char buffer[4096];

    TCPSocket projector(address,port);

    // We want to write: 02H 00H 00H 00H 00H 00H 02H
    // to the projector. Converting this to decimal. 
    unsigned char toSend[] = { (unsigned char)2, 0, 0, 0, 0, (unsigned char)2 }; 
    projector.write((char*) toSend, 6);

    projector.read((char *)buffer, 4096);
    std::cout << "Projector responded: " << (int)buffer[0] << "," << buffer[1] << std::endl; 
    projector.close();
    return 0;

}
