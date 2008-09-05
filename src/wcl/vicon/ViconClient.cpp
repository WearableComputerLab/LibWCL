/*
 * Copyright (c) 2008
 * Michael Marner <marnermr@cs.unisa.edu.au>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */



/*
 * Vicon Interface.
 *
 * Michael Marner (michael.marner@unisa.edu.au)
 */

#include "ViconClient.h"
#include <iostream>

using namespace std;
using namespace vicon;


ViconClient::ViconClient(std::string hostname, int port)
{
	this->socket = new TCPSocket(hostname, port);
	socket->setBlockingMode(socket->BLOCKING);
	loadTrackedObjects();
	update();
}

ViconClient::~ViconClient()
{
	socket->close();
	delete this->socket;
}

std::vector<std::string> ViconClient::getChannelNames()
{
	std::vector<std::string> list;
	
	//This is taken from the example source...
	int info = ViconClient::INFO;
	int request = ViconClient::REQUEST;
	if (!socket->isValid())
	{
		throw std::string("Error: Trying to get channel names, but socket is not valid");
	}
	//send it an info request...
	socket->write(&info, 4);
	socket->write(&request, 4);

	int packet;
	int type;
	
	socket->read(&packet, 4);
	socket->read(&type, 4);
	
	//make sure we're getting the right data back!
	if (packet == ViconClient::INFO && type == ViconClient::REPLY)
	{
		int numChannels;
		socket->read(&numChannels, 4);

		for (int i=0;i<numChannels;i++)
		{
			list.push_back(readChannel());
		}
		return list;		
		
	}
	return list;
}


void ViconClient::loadTrackedObjects()
{
	// Make a request for info...
	int info = ViconClient::INFO;
	int request = ViconClient::REQUEST;
	if (!socket->isValid())
	{
		throw std::string("Socket is not open!");
	}
	
	//send it an info request...
	socket->write(&info, 4);
	socket->write(&request, 4);
	
	int packet;
	int type;
	
	socket->read(&packet, 4);
	socket->read(&type, 4);
	
	//make sure we're getting the right data back!
	if (packet == ViconClient::INFO && type == ViconClient::REPLY)
	{
		
		int numChannels;
		socket->read(&numChannels, 4);
		std::string prevName;
		int channelPerNameCount = 0;
		for (int i=0;i<numChannels;i++)
		{
			/*
			 * This is a real pain.
			 * Because Vicon gives each DOF its own channel name, we need
			 * to do some processing to work out how many markers and 6Dofs
			 * we actually have.
			 */
			std::string channel = readChannel();

			//The real name is the first part of the name, so we can leave anything beyond the space
			channel = channel.substr(0, channel.find(" "));
			
			if (channel != "Time") {

				//This covers the first run through, where we have no previous channel name
				if (prevName.length() == 0) {
					prevName = channel;
					channelPerNameCount++;
				}

				// If this name is the same as the last one, just increment the count.
				// We need the count so we can decide if we are adding a marker or a 6dof.
				else if (prevName == channel) {
					channelPerNameCount++;
				}
				// The channel name has changed! this means we need to add something
				else {
					//Decide on whether it was a marker or a 6dof
					if (channelPerNameCount == 4) {
						TrackedObject m(prevName, MARKER);
						objects.push_back(m);
					}
					else if (channelPerNameCount == 6) {
						TrackedObject sdo(prevName, SIX_DOF);
						objects.push_back(sdo);
					}
					else {
						throw std::string("Unexpected number of DOF");
					}
					//reset for the next run through the loop
					prevName = channel;
					channelPerNameCount = 1;
				}
			}	
		}
		//add the last one...
		if (channelPerNameCount == 4) {
			TrackedObject m(prevName, MARKER);
			objects.push_back(m);
		}
		else if (channelPerNameCount == 6) {
			TrackedObject sdo(prevName, SIX_DOF);
			objects.push_back(sdo);
		}
		else {
			throw std::string("Unexpected number of DOF");
		}
	}
}

TrackedObject* ViconClient::getObject(std::string name) {
	
	for (unsigned int i=0;i<objects.size();i++) {
		if (objects[i].getName() == name) {
			return &objects[i];
		}
	}
	return 0;
	
}


void ViconClient::update()
{
	int data = ViconClient::DATA;
	int request = ViconClient::REQUEST;
	
	//Send a data request to the server.
	socket->write(&data, 4);
	socket->write(&request, 4);
	
	int packet;
	int type;
	
	socket->read(&packet, 4);
	socket->read(&type, 4);
	
	if (packet == ViconClient::DATA && type == ViconClient::REPLY) {
		
		int count;
		socket->read(&count, 4);
		
		double values[count];
		
		// read all values in one big block
		socket->read(values, sizeof((*values)*count));
		
		time = values[0];
		int offset = 1;
		for (unsigned int i=0;i<objects.size();i++) {
			objects[i].updateData(values, offset);
		}
	}
	else {
		throw std::string("Broken Data packet");
	}
}

std::string ViconClient::readChannel() 
{
	if (socket->isValid())
	{
		int letterCount;
		socket->read(&letterCount, 4);

		char name[letterCount+1];

		socket->read(name, letterCount);
		name[letterCount] = '\0';
		
		return std::string(name);
	}
	return 0;
}

