/*
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
 * Vicon Interface.
 *
 * Michael Marner (michael.marner@unisa.edu.au)
 */

#include "config.h"
#include "ViconClient.h"
#include <iostream>

using namespace std;
using namespace wcl;


/**
 * Reverses the order of bytes in an int.
 * Needed for Power PC
 *
 * @param The int to reverse.
 * @return The int with the byte order reversed.
 */
int32_t ViconClient::reverseByteOrder(int32_t n)
{
	unsigned char c1,c2,c3,c4;
	c1 = n & 255;
	c2 = (n >> 8) & 255;
	c3 = (n >> 16) & 255;
	c4 = (n >> 24) & 255;

	return ((int32_t)c1 << 24) + ((int32_t)c2 << 16) + ((int32_t)c3 << 8) + c4;
}

/**
 * Reverses the byte order of a double.
 * Needed for PowerPC
 *
 * @param n The double to reverse.
 * @return The double with reveresed byte order.
 */
double ViconClient::reverseBytesDouble(double n)
{
	union
	{
		double f;
		unsigned char b[8];
	} dat1, dat2;

	dat1.f = n;
	dat2.b[0] = dat1.b[7];
	dat2.b[1] = dat1.b[6];
	dat2.b[2] = dat1.b[5];
	dat2.b[3] = dat1.b[4];
	dat2.b[4] = dat1.b[3];
	dat2.b[5] = dat1.b[2];
	dat2.b[6] = dat1.b[1];
	dat2.b[7] = dat1.b[0];
	return dat2.f;
}


// If we are on PowerPC, reverse the byte order of all the packet types
#ifdef WORDS_BIGENDIAN
int32_t ViconClient::CLOSE = 0;
int32_t ViconClient::INFO = reverseByteOrder(1);
int32_t ViconClient::DATA = reverseByteOrder(2);
int32_t ViconClient::STREAMING_ON = reverseByteOrder(3);
int32_t ViconClient::STREAMING_OFF = reverseByteOrder(4);
int32_t ViconClient::REQUEST = 0;
int32_t ViconClient::REPLY = reverseByteOrder(1);
#else
int32_t ViconClient::CLOSE = 0;
int32_t ViconClient::INFO = 1;
int32_t ViconClient::DATA = 2;
int32_t ViconClient::STREAMING_ON = 3;
int32_t ViconClient::STREAMING_OFF = 4;
int32_t ViconClient::REQUEST = 0;
int32_t ViconClient::REPLY = 1;
#endif

ViconClient::ViconClient(std::string hostname, int port)
{

	this->socket = new TCPSocket(hostname, port);
	socket->setBlockingMode(socket->BLOCKING);
	loadTrackedObjects();

	isStreaming = false;
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
	if (!socket->isValid())
	{
		throw std::string("Error: Trying to get channel names, but socket is not valid");
	}
	//send it an info request...
	int32_t request[] = {ViconClient::INFO, ViconClient::REQUEST};

	socket->writeUntil(request, 8);

	int32_t packet;
	int32_t type;
	
	socket->readUntil(&packet, 4);
	socket->readUntil(&type, 4);


	//make sure we're getting the right data back!
	if (packet == ViconClient::INFO && type == ViconClient::REPLY)
	{
		int32_t numChannels;
		socket->readUntil(&numChannels, 4);

		#ifdef WORDS_BIGENDIAN
		numChannels = reverseByteOrder(numChannels);
		#endif

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
	int32_t info = ViconClient::INFO;
	int32_t request = ViconClient::REQUEST;
	if (!socket->isValid())
	{
		throw std::string("Socket is not open!");
	}
	
	//send it an info request...
	socket->writeUntil(&info, 4);
	socket->writeUntil(&request, 4);
	
	int32_t packet;
	int32_t type;
	
	socket->readUntil(&packet, 4);
	socket->readUntil(&type, 4);

	
	//make sure we're getting the right data back!
	if (packet == ViconClient::INFO && type == ViconClient::REPLY)
	{
		
		int32_t numChannels;
		socket->readUntil(&numChannels, 4);

		#ifdef WORDS_BIGENDIAN
		numChannels = reverseByteOrder(numChannels);
		#endif

		//cout << "We are expecting " << numChannels << " channels" << endl;

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
	else
	{
		throw std::string("Incorrect Packet/type when reading channel names");
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
	if (!isStreaming)
	{
		//turn on streaming yeah!
		int32_t data = ViconClient::STREAMING_ON;
		int32_t request = ViconClient::REQUEST;
		socket->writeUntil(&data, 4);
		socket->writeUntil(&request, 4);
		isStreaming = true;
	}

	int32_t packet[2];
	socket->readUntil(packet, 8);

	if (packet[0] == ViconClient::DATA && packet[1] == ViconClient::REPLY)
	{
	
		int32_t count;
		socket->readUntil(&count, 4);

		#ifdef WORDS_BIGENDIAN
		count = reverseByteOrder(count);
		#endif

		double values[count];

		// read all values in one big block
		socket->readUntil(values, 8*count);

		int offset = 1;
		for (unsigned int i=0;i<objects.size();i++) {
			objects[i].updateData(values, offset);
		}
	}
	else 
	{
		throw std::string ("Unexpected packet type");
	}
}

std::string ViconClient::readChannel() 
{
	if (socket->isValid())
	{
		int32_t letterCount;
		socket->readUntil(&letterCount, 4);

		//cout << "The length of this channel name is " << letterCount << " letters" << endl;

		#ifdef WORDS_BIGENDIAN
		letterCount = reverseByteOrder(letterCount);
		#endif

		char name[letterCount+1];

		socket->readUntil(name, letterCount);
		name[letterCount] = '\0';
		
		//cout << "New channel is " << std::string(name) << endl;
		return std::string(name);
	}
	return 0;
}

