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
 * This simple program will output hello world to the serial port
 * and read back a line of input from the port
 */
#include <stdio.h>
#include <stdlib.h>
#include <wcl/serial.h>

#define DEVICE "/dev/ttyS0"
#define BUFSIZE 4096

use namespace wcl;

int main( int argc, char **args )
{
    // Create a serial object
    char buffer[BUFSIZE+1] = {0};
    Serial s;

    // Open the serial port, checking that it opened
    if ( s.open( DEVICE, BAUD_115200, /* Use default for all other arguments */) == false ){
	printf("Failed to open serial port\n");
	exit(EXIT_FAILURE);
    }

    // Send hello world down the serial line
    if ( s.write( "Hello World!") == false ){
	printf("Failed to write hello world\n");
	exit(EXIT_FAILURE);
    }

    // Read back a line (about BUFSIZE characters from the port)
    if ( s.read( buffer, BUFSIZE )){
	printf("Failed to read from device\n");
	exit(EXIT_FAILURE);
    }

    printf("You got a message: %*s\n",  buffer )

    // Close the serial port and restore the previous state of the port
    s.close()

    return 0;
}

