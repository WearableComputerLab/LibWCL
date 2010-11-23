/*
 * Copyright (c) 2010 Michael Marner <michael@20papercups.net>
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
#include "LazySusan.h"

namespace wcl
{

	LazySusan::LazySusan(std::string device)
	{
		// I think the defaults are fine...
		mConnection.open(device.c_str(),
				         Serial::BAUD_115200,
						 Serial::DB_EIGHT,
						 Serial::NONE,
						 Serial::ONE,
						 Serial::RAW,
						 Serial::DISABLED,
						 Serial::NONBLOCKING,
						 Serial::BOTH);

	}

	LazySusan::~LazySusan()
	{
		mConnection.close();
	}


	void LazySusan::update()
	{
		using namespace std;

		static bool responsePending = false;
		static int bytesRead = 0;
		static char response[7];
		static int noReadCount = 0;
		response[6] = '\0';

		if (!responsePending)
		{
			std::string request = "p\r";
			mConnection.write(request);
			mConnection.drain();
			responsePending = true;
			bytesRead = 0;
			noReadCount = 0;
		}

		bytesRead += mConnection.read(response+bytesRead, 7 - bytesRead);

		if (bytesRead == 0)
			noReadCount++;

		if (noReadCount > 10)
		{
			responsePending = false;
		}

		if (bytesRead == 7)
		{
			cout << "Response: " << response << endl;
			// null out the dollar sign/newline char
			response[4] = '\0';
			responsePending = false;
			bytesRead = 0;
			// convert hex to decimal...
			unsigned int x;   
			std::stringstream ss;
			ss << std::hex << response;
			ss >> x;
			double d = ((double) x)/4096;
			cout << "Degrees: " << d*360 << endl;
			mTrackedObject.setRotation(d);
		}
	}

	TrackedObject* LazySusan::getObject(std::string name)
	{
		return &mTrackedObject;
	}

	std::vector<TrackedObject* > LazySusan::getAllObjects()
	{
		static std::vector<TrackedObject*> objects;
		static bool filled = false;

		if (!filled)
		{
			objects.push_back(&mTrackedObject);
			filled = true;
		}

		return objects;
	}

	void LazySusan::setUnits(Units u) {}

};
