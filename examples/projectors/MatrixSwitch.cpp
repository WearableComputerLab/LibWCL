/*
 * Copyright (c) <2011>, Michael Marner <michael@20papercups.net>
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
#include <wcl/projector/MatrixSwitch.h>

using namespace wcl;
using namespace std;

void usage() {
	cout << "Usage: " << endl;
	cout << "switch [switch IP] [switch ID] [source] [destination]" << endl;
}


int main(int argc, char** argv) {


    // Determine whethe the arguments given are valid. 
    if ( argc == 5 ) 
	{
		std::string ip = argv[1];
		unsigned int id = atoi(argv[2]);
		unsigned int input = atoi(argv[3]);
		unsigned int output = atoi(argv[4]);

		MatrixSwitch sw(ip, id);
		cout << "Attempting to route signal from " << input << " to " << output << endl;
		bool res = sw.route(input, output);
		if (res)
			cout << "Success!" << endl;
		else
			cout << "Could not route!" << endl;
    } 
	else 
	{
        usage();
        return -1;
    }

    return 0;
}
