/*
 * Vicon Interface.
 *
 * Michael Marner (michael.marner@unisa.edu.au)
 */

#include "ViconClient.h"
#include <iostream>

using namespace std;


ViconClient::ViconClient(std::string hostname, int port = 800)
{
	this->socket = new TCPSocket(hostname, port);
	socket->setBlockingMode(socket->BLOCKING);
	loadTrackedObjects();
	getData();
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
			std::string channel = readChannel();
			channel = channel.substr(0, channel.find(" "));
			
			if (channel != "Time") {
				
				if (prevName.length() == 0) {
					prevName = channel;
					channelPerNameCount++;
				}
				else if (prevName == channel) {
					channelPerNameCount++;
				}
				else {
					if (channelPerNameCount == 4) {
						Marker * m = new Marker(prevName);
						objects.push_back(m);
					}
					else if (channelPerNameCount == 6) {
						SixDofObject * sdo = new SixDofObject(prevName);
						objects.push_back(sdo);
					}
					else {
						throw std::string("Unexpected number of DOF");
					}
					prevName = channel;
					channelPerNameCount = 1;
				}
			}	
		}
		//add the last one...
		if (channelPerNameCount == 4) {
			Marker * m = new Marker(prevName);
			objects.push_back(m);
		}
		else if (channelPerNameCount == 6) {
			SixDofObject * sdo = new SixDofObject(prevName);
			objects.push_back(sdo);
		}
		else {
			throw std::string("Unexpected number of DOF");
		}
	}
}

TrackedObject* ViconClient::getObject(std::string name) {
	
	for (unsigned int i=0;i<objects.size();i++) {
		if (objects[i]->getName() == name) {
			return objects[i];
		}
	}
	return 0;
	
}


void ViconClient::update()
{
	int data = ViconClient::DATA;
	int request = ViconClient::REQUEST;
	
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
			objects[i]->updateData(values, offset);
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

