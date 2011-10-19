/*-
 * Copyright (c) 2011 Tim Simon <tim.simon@radiumblue.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "RemoteProjector.h"

#include <iostream>

namespace wcl {

/**
 * Default constructor.  
 */
RemoteProjector::RemoteProjector()
{}

/**
 * Create a RemoteProjector and attempt to connect the socket to the specified
 * server on the given port.
 *
 * @param address Address of the projector to connect to, this can be a string
 * based ip, or hostname
 * @param port The remote port to connect to
 * @throws SocketError If there was a problem resolving the hostname or connecting to the host
 */
RemoteProjector::RemoteProjector( const std::string &server, const unsigned port, bool autoConnect) throw (SocketException)
{
    rprojector = new TCPSocket(server, port, true);    
}

void RemoteProjector::turnOn() {
    // 02H 00H 00H 00H 00H 00H 02H
    unsigned char toSend[] = { (unsigned char)2, 0, 0, 0, 0, (unsigned char)2 };  
    rprojector->write((char*) toSend, 6); 
}

void RemoteProjector::turnOff() {
    // 02H 01H 00H 00H 00H 03H 
    unsigned char toSend[] = { (unsigned char)2, (unsigned char)1, 0, 0, 0, (unsigned char)3 };  
    rprojector->write((char*) toSend, 6); 
}
void RemoteProjector::getResponse() {
    // Read the response from the projector. 
    unsigned char buffer[4096];
    rprojector->read((char *)buffer, 4096);
    if ( (int) buffer[0] == 34  ) { 
        std::cout << "Projecter sends ACK: Doing task." << std::endl;
    } else {
        std::cout << "Projector did not send ACK. Recieved: " << (int)buffer[0] << std::endl;
    }   

}
}; // namespace wcl
