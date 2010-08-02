/*-
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
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


/*
 * Simple test of the Vicon client.
 *
 * @author Michael Marner <marnermr@cs.unisa.edu.au>
 *
 */


// include system headers
#include <istream>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <signal.h>

// include our headers
#include <wcl/tracking/ViconClient.h>
#include <wcl/tracking/TrackedObject.h>

#define MARKER 1  // XXX At present the value for Marker is not known -benjsc 20100802 (Micheal should know it)

using namespace std;
using namespace wcl;


/**
 * Global client.
 * This is just a demo program so we are allowed to use
 * bad practices like this!
 */
ViconClient* client;


/**
 * Displays usage information
 */
void printUsage()
{
	cout << "Vicon Client Example" << endl;
	cout << "Usage: vicontest [host] [port]" << endl;
	cout << "Port is optional, host is not" << endl;
}


/**
 * Interrupt handler for when someone presses CTRL-C.
 * Means we can shutdown the client cleanly
 */
void termHandler(int a)
{
	//asdf
	cout << "cleeaaaan shutdown" <<endl;
	delete client;
	//exit(0);
}


/**
 * Entry point
 */
int main(int argc, char** argv)
{
	// register the signal handler
	//signal(SIGINT, termHandler);

	try
	{

		// Use the supplied port
		if (argc == 3)
		{
			client = new ViconClient(argv[1], atoi(argv[2]));
		}

		// Use the default port
		else if (argc == 2)
		{
			client = new ViconClient(argv[1]);
		}

		// display usage and exit
		else
		{
			printUsage();
			return 1;
		}


		// lets print out the list of objects
		vector<ViconTrackedObject> &objects = client->objects;

		cout << endl << "*** Available Tracked Objects ***" << endl << endl;

		for (unsigned i=0;i<objects.size();i++)
		{
			if (objects[i].getType() == MARKER)
			{
				cout << "Marker: ";
			}
			else
			{
				cout << "6DOF:   ";
			}
			cout << objects[i].getName() << endl;
		}

		// now lets just loop, printing out the current positions/rotation of each object.
		
		cout << endl << "*** Entering update loop ***" << endl << endl;

		while (true)
		{
			client->update();

			for (unsigned i=0;i<objects.size();i++)
			{
				cout << objects[i].toString() << endl;
			}

			usleep(500);
		}
	}
	catch(string s)
	{
		cout << s << endl;
	}

	delete client;
	return 0;
}


