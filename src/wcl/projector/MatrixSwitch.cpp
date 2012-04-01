/*-
 * Copyright (c) 2012 Michael Marner <michael@20papercups.net>
 *
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


#include "MatrixSwitch.h"


namespace wcl
{

	MatrixSwitch::MatrixSwitch(const std::string& ipAddress, unsigned int switchID)
		: mIP(ipAddress), mSwitchID(switchID)
	{
		// nothing to do here...
	}

	MatrixSwitch::~MatrixSwitch()
	{
		// nothing to do here either...
	}


	bool MatrixSwitch::route(unsigned int inputID, unsigned int outputID)
	{
		// note: I don't know how many simultaneous connections
		// the matrix switch supports. So, rather than connect
		// on creation and disconnect on destruction, we'll just
		// create a socket each method call.

		if (inputID < 1 && inputID >32 && outputID < 1 && outputID > 32)
			return false;

		// connects on port 23 always
		wcl::TCPSocket s(mIP, 23);

		char message[14];
		message[13] = 0;

		sprintf(message, "*%3d0004%2d%2d!", mSwitchID, outputID, inputID);
		s.writeUntil(message, 13);
		s.close();
	}


	bool MatrixSwitch::disconnect(unsigned int outputID)
	{
		if (outputID < 1 && outputID > 32)
			return false;

		// connects on port 23 always
		wcl::TCPSocket s(mIP, 23);

		char message[14];
		message[13] = 0;

		sprintf(message, "*%3d0004%2d%2d!", mSwitchID, outputID, 0);
		s.writeUntil(message, 13);
		s.close();
	}

};

