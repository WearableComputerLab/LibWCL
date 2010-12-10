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


#include <assert.h>
#include <sstream>
#include <iostream>
#include <wcl/tracking/Polhemus.h>
#include <wcl/tracking/TrackedObject.h>
#include <wcl/Exception.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

namespace wcl
{

	Polhemus::Polhemus(std::string path, TrackerType t) : Tracker(),  activeSensorCount(-1), type(t),continuous(false)
	{
		std::cout << "About to connect to " << path << std::endl;
		if (!connection.open(path.c_str(), Serial::BAUD_115200))
		{
			throw Exception(strerror(errno));
		}
		usleep(2000);
		connection.setBlockingMode(Serial::BLOCKING);
	
		setContinuous(false);
		usleep(2000);
		// clear whatever existing half completed commands exist
		if (!connection.flush())
		{
			throw Exception(strerror(errno));
		}
		
		
		clearInput();
		setAsciiOutput();
		setDataFormat();
		setUnits(MM);


		clearInput();

		if (type == PATRIOT)
		{
			sensors.push_back(PolhemusTrackedObject("0"));
			sensors.push_back(PolhemusTrackedObject("1"));
			//getSensorCount();
		}
		else
		{
			sensors.push_back(PolhemusTrackedObject("0"));
			sensors.push_back(PolhemusTrackedObject("1"));
			sensors.push_back(PolhemusTrackedObject("2"));
			sensors.push_back(PolhemusTrackedObject("3"));
		}

		//setContinuous(true);
		//printf("connection.getAvailableCount init: %d\n",connection.getAvailableCount());
	}

	Polhemus::~Polhemus()
	{
		if (continuous)
			setContinuous(false);

		connection.close();
	}

	void Polhemus::clearInput()
	{
		connection.drain();
		connection.flush();
		int bytesAvailable = connection.getAvailableCount();
		char* rubbish = new char[bytesAvailable];
		connection.read((void*) rubbish, bytesAvailable);
		delete[] rubbish;
	}

	void Polhemus::update()
	{
		char response[70] = {0};
		double x, y, z, rx, ry, rz, rw;
		unsigned number;
		int expectedBytes=0;
		if (type == PATRIOT)
			expectedBytes = 69;
		else if (type == FASTRAK)
			expectedBytes = 54;


		if (activeSensorCount > 0)
		{
			if (!continuous)
			{
				connection.write("P");
				connection.drain();
			}
			
			/*
			char response[140];
			memset(response, 0, 140);
			size_t sz = connection.getAvailableCount();
			connection.readUntil(response, 138);
			printf("%d:%s\n", sz, response);
			*/
	

			for (int i=0; i<activeSensorCount; i++)
			{
				connection.readUntil((void*) &response, expectedBytes);
				response[expectedBytes] = '\0';
					
				//printf("%d %s\n", sz, response);
				
				int count = sscanf(response, "%u%lf%lf%lf%lf%lf%lf%lf", &number, &x, &y, &z, &rw, &rx, &ry, &rz);
				if( count != 8 )
				{
					std::stringstream ss;
					ss << "Invalid message received from polhemus: " << response;
				    throw Exception(ss.str());
				}
				assert (number <= sensors.size());
				
				//printf("%d: (%f,%f,%f)\n", number, x, y, z);
				
				if (units == MM)
				{
					sensors[number-1].update(x*10,y*10,z*10,rw,rx,ry,rz);
				}
				else
				{
					sensors[number-1].update(x,y,z,rw,rx,ry,rz);
				}
				
			}
		}
		else
		{
			throw Exception("Could not figure out how many sensors are connected to polhemus");
		}
	}

	TrackedObject* Polhemus::getObject(std::string name)
	{
		if (name == "sensor1" && activeSensorCount >= 1)
			return &sensors[0];
		else if (name == "sensor2" && activeSensorCount >= 2)
			return &sensors[1];
		else if (name == "sensor3" && activeSensorCount >= 3)
			return &sensors[2];
		else if (name == "sensor4" && activeSensorCount >= 4)
			return &sensors[3];
		else
			throw Exception("Invalid tracked object name");
	}

	std::vector<TrackedObject *> Polhemus::getAllObjects()
	{
	    std::vector<TrackedObject *> objects;

	    for(int i=0; i< activeSensorCount; i++ ){
		objects.push_back(&sensors[i]);
	    }

	    return objects;
	}

	
	void Polhemus::setHemisphere(const wcl::Vector& hemisphere)
	{
		std::stringstream command1, command2, command3, command4;
		command1 << "H1," << hemisphere[0] << "," << hemisphere[1] << "," << hemisphere[2] << "\r";
		command2 << "H2," << hemisphere[0] << "," << hemisphere[1] << "," << hemisphere[2] << "\r";
		connection.write(command1.str());
		connection.write(command2.str());
		if (type == FASTRAK)
		{
			command3 << "H3," << hemisphere[0] << "," << hemisphere[1] << "," << hemisphere[2] << "\r";
			command4 << "H4," << hemisphere[0] << "," << hemisphere[1] << "," << hemisphere[2] << "\r";
			connection.write(command3.str());
			connection.write(command4.str());
		}
		std::cout << command1.str() ;
	}

	void Polhemus::setUnits(Units u)
	{
		this->units = u;
		int bytesWritten = 0;
		int expected = 0;
		if (type == PATRIOT)
		{
			expected = 3;
			switch (u)
			{
				case INCHES:
					bytesWritten = connection.write("U0\r");
					break;
				case CM:
				case MM:
					bytesWritten = connection.write("U1\r");
					break;
			}
		}
		else
		{
			expected = 1;
			switch (u)
			{
				case INCHES:
					bytesWritten = connection.write("U");
					break;
				case CM:
				case MM:
					bytesWritten = connection.write("u");
					break;
			}
		}
		if (bytesWritten != expected)
		{
			std::stringstream ss;
			ss << "ERROR: Could not set units, expected to write " << expected << " bytes but only wrote " << bytesWritten;
			throw Exception(ss.str());
		}
	}


	void Polhemus::setAlignmentFrame(const wcl::Vector& origin, const wcl::Vector& xPos, const wcl::Vector& yPos)
	{
		using namespace std;
		std::cout << "in setAlignmentFrame" << std::endl;
		assert (origin.getRows() == 3);
		assert (xPos.getRows() == 3);
		assert (yPos.getRows() == 3);
		cout << "finished asserts" << endl;

		int numSensors = 0;
		if (type == PATRIOT)
			numSensors = 2;
		else if (type == FASTRAK)
			numSensors = 4;

		for (int i=1;i<=numSensors;++i)
		{
			cout << "about to clear alignment frame for sensor" << i << endl;
			//first we reset...
			if (type == PATRIOT)
			{
				char msg[4];
				sprintf(msg, "%d\r", i);
				connection.write(msg, 3);
			}
			else if (type == FASTRAK)
			{
				char msg[4];
				//subtle difference is subtle
				sprintf(msg, "R%d\r", i);
				connection.write(msg, 3);
			}

			//now we set the reference frame
			std::stringstream ss;
			ss << "A" << i << ",";
			ss << origin[0] << "," << origin[1] << ","<< origin[2] << ",";
			ss << xPos[0] << "," << xPos[1] << ","<< xPos[2] << ",";
			ss << yPos[0] << "," << yPos[1] << ","<< yPos[2] << "\r";

			std::cout << "about to write " << ss.str() << std::endl;
			connection.write(ss.str());
		}
	}

	void Polhemus::setAsciiOutput()
	{
		int bytesWritten = 0;
		int expected = 0;
		if (type == PATRIOT)
		{
			bytesWritten = connection.write("F0\r");
			expected = 3;
		}
		else
		{
			expected = 1;
			bytesWritten = connection.write("F");
		}
		if (bytesWritten != expected)
		{
			std::stringstream ss;
			ss << "ERROR: Could not set ascii output, expected to write " << expected << " bytes but only wrote " << bytesWritten;
			throw Exception(ss.str());
		}
	}

	void Polhemus::setContinuous(bool c)
	{
		switch (type)
		{
			case PATRIOT:
				if (c)
					connection.write("C\r");
				else
					connection.write("P");
				break;
			case FASTRAK:
				if (c)
					connection.write("C");
				else
					connection.write("c");
				break;
		}
		continuous = c;
	}

	void Polhemus::setDataFormat()
	{
		// we are going to ask for cartesian coordinates for position
		// and a quaternion for rotation, with a CRLF separating stations
		int bytesWritten = 0;
		int expected = 0;
		if (type == PATRIOT)
		{
			expected = 9;
			bytesWritten = connection.write("O*,2,7,1\r");
		}
		else
		{
			expected = 10;
			bytesWritten = connection.write("O1,2,11,1\r");
			bytesWritten = connection.write("O2,2,11,1\r");
			bytesWritten = connection.write("O3,2,11,1\r");
			bytesWritten = connection.write("O4,2,11,1\r");
		}

		if (bytesWritten != expected)
		{
			std::stringstream ss;
			ss << "ERROR: Could not set ascii output, expected to write " << expected << " bytes but only wrote " << bytesWritten;
			throw Exception(ss.str());
		}
	}

	void Polhemus::readAll(std::string prefix)
	{
		unsigned char response[1024] = {0};
		response[1023] = '\0';
		connection.read((void*) &response, 1024);
		std::cout << prefix << " (" << response << ")" << std::endl;

	}

	void Polhemus::setSensorCount(int c)
	{
		if (c > 0)
		{
			this->activeSensorCount = c;
		}
	}

	void Polhemus::getSensorCount()
	{
		if (type == PATRIOT)
		{
			int bytesWritten = connection.write("0\r");
			if (bytesWritten != 3)
			{
				std::stringstream ss;
				ss << "ERROR: Could not get sensor count, expected to write 3 bytes but only wrote " << bytesWritten;
				throw Exception(ss.str());
			}
			unsigned char response[16] = {0};
			response[15] = '\0';
			int responseLength = connection.read((void*) &response, 15);
			if (responseLength != 15)
			{
				std::stringstream ss;
				ss << "ERROR: Could not get sensor count, expected to read 15 bytes but only read " << responseLength;
				//throw std::string(strerror(errno));
				//throw std::string(ss.str());
			}
			//std::cout << "Response from tracker in getSensorCount: " << response << std::endl;
			if (response[12] == '3')
			{
				activeSensorCount = 2;
			}
			else if (response[12] == '1')
			{
				activeSensorCount = 1;
			}
			else
			{
				throw Exception("Michael needs to debug the getsensorcount code!");
			}
		}
		else
		{
			throw Exception("Only patriot supports getSensorCount");
		}
	}
}

