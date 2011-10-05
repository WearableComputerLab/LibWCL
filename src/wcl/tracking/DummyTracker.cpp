/*
 * Copyright (c) 2011 Markus Broecker <markus.broecker@unisa.edu.au>
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
#include "DummyTracker.h"
#include "DummyTrackedObject.h"

using namespace wcl;

DummyTracker::~DummyTracker()
{
	for (TrackedObjectList::iterator it = trackedObjects.begin(); it != trackedObjects.end(); ++it)
	{
		delete it->second;
	}
}

void DummyTracker::update()
{
}

TrackedObject* DummyTracker::getObject(std::string name)
{
	return trackedObjects[name];
}

std::vector<TrackedObject*> DummyTracker::getAllObjects()
{
	std::vector<TrackedObject*> objects;
	for (TrackedObjectList::const_iterator it = trackedObjects.begin(); it != trackedObjects.end(); ++it)
		objects.push_back(it->second);
		
	return objects;
}