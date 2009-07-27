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

namespace wcl
{

	Patriot::Patriot(std::string path)
	{
		std::cout << "About to connect to " << path << std::endl;
		connection.setBlockingMode(Serial::BLOCKING);
		if (!connection.open(path.c_str(), Serial::BAUD_115200))
		{
			throw std::string(strerror(errno));
		}
		// clear whatever existing half completed commands exist
		setAsciiOutput();
		setUnits(MM);
		setDataFormat();
		//getSensorCount();
		activeSensorCount = 2;
		setContinuous();
	}

	Patriot::~Patriot()
	{
		connection.close();
	}

	void Patriot::update()
	{
		char response[68];
		double x, y, z, rx, ry, rz, rw;
		int number;

		if (activeSensorCount > 0)
		{
			connection.read((void*) &response, 67);
			response[67] = '\0';
			std::cout << response << std::endl;
			scanf(response, "%d%f%f%f%f%f%f%f", &number, &x, &y, &z, &rw, &rx, &ry, &rz);
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
			scanf(response, "%d%f%f%f%f%f%f%f", &number, &x, &y, &z, &rw, &rx, &ry, &rz);
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
		switch (u)
		{
			case INCHES:
				connection.write("U0\r\n");
				break;
			case CM:
			case MM:
				connection.write("U1\r\n");
				break;
		}
	}

	void Patriot::setAsciiOutput()
	{
		connection.write("F0\r\n");
	}

	void Patriot::setContinuous()
	{
		connection.write("C\r\n");
	}

	void Patriot::setDataFormat()
	{
		// we are going to ask for cartesian coordinates for position
		// and a quaternion for rotation, with a CRLF separating stations
		connection.write("O*,2,7,1\r\n");
	}

	void Patriot::getSensorCount()
	{
		unsigned char ctrlU = 0x15;
		connection.write((void*) &ctrlU, 1);
		connection.write("0\r\n", 2);
		connection.drain();
		unsigned char response[16];
		response[15] = '\0';
		connection.read((void*) &response, 15);
		std::cout << response << std::endl;
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

