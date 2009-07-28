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
#include <wcl/tracking/Patriot.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

namespace wcl
{

	Patriot::Patriot(std::string path)
	{
		std::cout << "About to connect to " << path << std::endl;
		if (!connection.open(path.c_str(), Serial::BAUD_115200))
		{
			throw std::string(strerror(errno));
		}
		connection.setBlockingMode(Serial::BLOCKING);
		// clear whatever existing half completed commands exist
		if (!connection.flush())
		{
			throw std::string(strerror(errno));
		}
		//std::cout << "Setting ascii output... ";
		//setAsciiOutput();
		//readAll("in setAscii");
		//std::cout << "DONE!" << std::endl;
		//std::cout << "Setting units ... ";
		//setUnits(MM);
		//readAll("in setunits");
		//std::cout << "DONE!" << std::endl;
		//std::cout << "Setting data format... ";
		//setDataFormat();
		//readAll("in setformat");
		//std::cout << "DONE!" << std::endl;
		//std::cout << "getting sensor count... ";
		getSensorCount();
		//activeSensorCount = 1;
		//readAll();
		//std::cout << "DONE!" << std::endl;
		std::cout << "Setting continuous... ";
		setContinuous();
		//readAll("In setContinuous");
		std::cout << "DONE!" << std::endl;
	}

	Patriot::~Patriot()
	{
		connection.close();
	}

	void Patriot::update()
	{
		char response[68] = {0};
		double x, y, z, rx, ry, rz, rw;
		int number;

		if (activeSensorCount > 0)
		{
			connection.read((void*) &response, 67);
			response[67] = '\0';
			std::cout << "Reading Update Response: " << response << std::endl;
			int result = scanf(response, "%d%f%f%f%f%f%f%f", &number, &x, &y, &z, &rw, &rx, &ry, &rz);
			if (units == MM)
			{
				sensor1.update(x*10,y*10,z*10,rw,rx,ry,rz);
			}
			else
			{
				sensor1.update(x,y,z,rw,rx,ry,rz);
			}
		}
		if (activeSensorCount > 1)
		{
			connection.read((void*) &response, 67);
			response[67] = '\0';
			int result =  scanf(response, "%d%f%f%f%f%f%f%f", &number, &x, &y, &z, &rw, &rx, &ry, &rz);
			if (units == MM)
			{
				sensor2.update(x*10,y*10,z*10,rw,rx,ry,rz);
			}
			else
			{
				sensor2.update(x,y,z,rw,rx,ry,rz);
			}
		}
	}

	TrackedObject* Patriot::getObject(std::string name)
	{
		if (name == "sensor1")
			return &sensor1;
		else if (name == "sensor2")
			return &sensor2;
		else
			throw std::string("Invalid tracked object name");
	}

	
	void Patriot::setHemisphere(Sensor s, const wcl::Vector& hemisphere)
	{
		std::stringstream command;
		if (s == SENSOR1)
			command << "h1,";
		else
			command << "h2,";

		command << hemisphere[0] << "," << hemisphere[1] << "," << hemisphere[2] << "\r";
	}

	void Patriot::setUnits(Units u)
	{
		this->units = u;
		int bytesWritten = 0;
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
		if (bytesWritten != 3)
		{
			std::stringstream ss;
			ss << "ERROR: Could not set units, expected to write 3 bytes but only wrote " << bytesWritten;
			throw std::string(ss.str());
		}
	}

	void Patriot::setAsciiOutput()
	{
		int bytesWritten = connection.write("F0\r");
		if (bytesWritten != 3)
		{
			std::stringstream ss;
			ss << "ERROR: Could not set ascii output, expected to write 4 bytes but only wrote " << bytesWritten;
			throw std::string(ss.str());
		}
	}

	void Patriot::setContinuous()
	{
		connection.write("C\r");
	}

	void Patriot::setDataFormat()
	{
		// we are going to ask for cartesian coordinates for position
		// and a quaternion for rotation, with a CRLF separating stations
		int bytesWritten = connection.write("O*,2,7,1\r");
		if (bytesWritten != 9)
		{
			std::stringstream ss;
			ss << "ERROR: Could not set ascii output, expected to write 9 bytes but only wrote " << bytesWritten;
			throw std::string(ss.str());
		}
	}

	void Patriot::readAll(std::string prefix)
	{
		unsigned char response[1024] = {0};
		response[1023] = '\0';
		int responseLength = connection.read((void*) &response, 1024);
		std::cout << prefix << " (" << response << ")" << std::endl;

	}
	void Patriot::getSensorCount()
	{
		int bytesWritten = connection.write("0\r", 3);
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
		std::cout << "Response from tracker in getSensorCount: " << response << std::endl;
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
}

