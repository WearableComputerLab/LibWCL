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

#include <wcl/tracking/VirtualTracker.h>

#include <netinet/in.h>
#include <stdint.h>

namespace wcl
{
	VirtualTracker::VirtualTracker(std::string host, unsigned int port)
	{
		socket = new wcl::TCPSocket(host, port);
	}

	VirtualTracker::~VirtualTracker()
	{
		delete socket;
	}

	void VirtualTracker::update()
	{
		int32_t objectCount=0;
		socket->readUntil(&objectCount, 4);
		objectCount = ntohl(objectCount);

		for (unsigned i=0;i<objectCount;i++)
		{
			char name[9];
			name[8] = '\0';
			socket->readUntil(name, 8);
			double data[7];
			VirtualTrackedObject* to;

			if ((objects.find(name)) == objects.end())
			{
				to = new VirtualTrackedObject(name);
				objects[name] = to;
			}
			else
			{
				to = objects[name];
			}
			socket->readUntil(data, 56);

			to->setData(data[0],
						data[1],
						data[2],
						data[3],
						data[4],
						data[5],
						data[6]);
		}
	}

	TrackedObject* VirtualTracker::getObject(std::string name)
	{
		if ((objects.find(name)) != objects.end())
		{
			return objects[name];
		}
		return NULL;
	}

}

