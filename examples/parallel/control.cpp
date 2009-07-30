/*-
 * Copyright (c) 2008 Benjamin Close <Benjamin.Close@clearchain.com>
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


/**
 * This program is a simple little test application which
 * allows the toggling of the DATA0 line and reading back
 * of state from the BUSY line. It was used to control the
 * Canon IXUS 400 digitial camera attached to snappy
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define LINUX
#include <wcl/parallel/Parallel.h>

#define ON true
#define OFF false

using namespace wcl;

/**
 * Poll pin 10 of the port to see 
 * if the camera is on or off
 */
bool isCameraOn( Parallel &port )
{
    return !port.getSignal(Parallel::BUSY);
}

/**
 * Pulse the power button by toggling the
 * parallel port.
 */
void hitButton(Parallel &port)
{
    port.setDataLine(Parallel::DATA0, OFF );
    port.setDataLine(Parallel::DATA0, ON );
    sleep(1);
    port.setDataLine(Parallel::DATA0, OFF );
    sleep(1);
}

int main(int argc, char **argv)
{
    Parallel port;
    bool ison;

    if(port.open(Parallel::LPT1)== false ){
	perror("Can't open LPT1");
	exit(1);
    }

    if ( argc ==1 ){
	ison = isCameraOn ( port );
	printf("Camera is %s\n" , ison ? "on" : "off");
	return ison;
    }

    if( argc > 1 ){
        printf("Toggling State\n");
        hitButton(port);
        ison = isCameraOn ( port );
        printf("Camera is %s\n" , ison ? "on" : "off");
        return ison;
    }

    return -127;
}

