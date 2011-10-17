/*
 * Copyright (c) <2011>, Tim Simon <tim.simon@radiumblue.net>
 * All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 *  Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 *  Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <stdio.h>
#include "wcl/network/TCPServer.h"

using namespace wcl;

void usage() {
    printf("NAME\n\tpcontrol - Network command controller for NEC projectors\n\n"
            "SYNOPSIS\n\t pcontrol [options] -a <address> -p <port> [COMMAND]\n\n"
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
            "COMMANDS\n[COMMAND] can be any single value of the following:"
            "\t--ON\nPower ON\n"
            "\t--OFF\nPower OFF\n"
          );
}


int main(int argc, char** argv) {

    std::string address = "127.0.0.1";    
    unsigned int port = 7142;

    // Determine whethe the arguments given are valid. 
    if ( argc >= 2 ) {

        for ( int counter = 0; counter < argc ; ++counter ) {

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

        // Now determine whether we actually got given a command to send to the projector or not. 
        if (( strcmp (argv[argc-1], "--ON" ) == 0 ) ||  ( strcmp (argv[argc-1], "--OFF" ) == 0 ) ) {
        } else {
            usage();
            return -1;
        }
                // 02H 01H 00H 00H 00H 03H 
                unsigned char toSend[] = { (unsigned char)2, (unsigned char)1, 0, 0, 0, (unsigned char)3 }; 
                projector.write((char*) toSend, 6); 
            }
        try {

            TCPSocket projector(address,port);
            // Get the last value, which should be our command, and use it for our command to run. 
            if ( strcmp (argv[argc-1], "--ON" ) == 0 ) {
                // 02H 00H 00H 00H 00H 00H 02H
                unsigned char toSend[] = { (unsigned char)2, 0, 0, 0, 0, (unsigned char)2 }; 
                projector.write((char*) toSend, 6);
            }
            if ( strcmp (argv[argc-1], "--OFF" ) == 0 ) {

                // 02H 01H 00H 00H 00H 03H 
                unsigned char toSend[] = { (unsigned char)2, (unsigned char)1, 0, 0, 0, (unsigned char)3 }; 
                projector.write((char*) toSend, 6); 
            }
            // Read the response from the projector. 
            unsigned char buffer[4096];
            projector.read((char *)buffer, 4096);
            std::cout << "Projector responded: " << (int)buffer[0] << "," << buffer[1] << std::endl; 
            projector.close();
        } catch (SocketException* e) {
            fprintf( stderr, "%s", e->what() ); 
        }

        }

        return 0;
    }
