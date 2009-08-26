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


#include <sstream>
#include <iostream>
#include <wcl/tracking/Polhemus.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

namespace wcl
{

	Polhemus::Polhemus(std::string path, TrackerType t) : type(t), activeSensorCount(-1), continuous(false)
	{
		std::cout << "About to connect to " << path << std::endl;
		if (!connection.open(path.c_str(), Serial::BAUD_115200))
		{
			throw std::string(strerror(errno));
		}
		usleep(2000);
		connection.setBlockingMode(Serial::BLOCKING);
		// clear whatever existing half completed commands exist
		if (!connection.flush())
		{
			throw std::string(strerror(errno));
		}
		setAsciiOutput();
		setDataFormat();
		setUnits(MM);
		setContinuous(true);

		if (type == PATRIOT)
		{
			sensors = new PolhemusTrackedObject[2];
			getSensorCount();
		}
		else
		{
			sensors = new PolhemusTrackedObject[4];
		}
	}

	Polhemus::~Polhemus()
	{
		if (continuous)
			setContinuous(false);

		connection.close();
		delete[] sensors;
	}

	void Polhemus::update()
	{
		char response[70] = {0};
		double x, y, z, rx, ry, rz, rw;
		int number;

		if (activeSensorCount > 0)
		{
			if (!continuous)
			{
				connection.write("P");
			}

			for (int i=0; i<activeSensorCount; i++)
			{
				if (type == PATRIOT)
				{
					connection.read((void*) &response, 69);
					response[69] = '\0';
				}
				else if (type == FASTRAK)
				{
					connection.read((void*) &response, 54);
					response[54] = '\0';
				}

				std::cout << "Reading Update Response: " << response << std::endl;
				int result = sscanf(response, "%d%lf%lf%lf%lf%lf%lf%lf", &number, &x, &y, &z, &rw, &rx, &ry, &rz);
				if (units == MM)
				{
					sensors[i].update(x*10,y*10,z*10,rw,rx,ry,rz);
				}
				else
				{
					sensors[i].update(x,y,z,rw,rx,ry,rz);
				}
			}
		}
		else
		{
			throw std::string("I don't know how many sensors there are!");
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
			throw std::string("Invalid tracked object name");
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
			throw std::string(ss.str());
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
			throw std::string(ss.str());
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
			throw std::string(ss.str());
		}
	}

	void Polhemus::readAll(std::string prefix)
	{
		unsigned char response[1024] = {0};
		response[1023] = '\0';
		int responseLength = connection.read((void*) &response, 1024);
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
				throw std::string(ss.str());
			}
			unsigned char response[16] = {0};
			response[15] = '\0';
			int responseLength = connection.read((void*) &response, 15);
			if (responseLength != 15)
			{
				std::stringstream ss;
				ss << "ERROR: Could not get sensor count, expected to read 15 bytes but only read " << responseLength;
				//throw std::string(strerror(errno));
				throw std::string(ss.str());
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
				throw std::string("Michael needs to debug the getsensorcount code!");
			}
		}
		else
		{
			throw std::string("Only patriot supports getSensorCount");
		}
	}
}

