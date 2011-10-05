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

#ifndef DUMMYTRACKER_H_SWS8XTZ5
#define DUMMYTRACKER_H_SWS8XTZ5

#include <wcl/tracking/Tracker.h>

#include <string>
#include <map>

namespace wcl
{

class DummyTrackedObject;
class DummyTracker : public Tracker
{
public:
	~DummyTracker();
	
	void update();

	void addTrackedObject(DummyTrackedObject*);

	TrackedObject* getObject(std::string name) = 0;
	std::vector<TrackedObject *> getAllObjects() = 0;
	void setUnits(Units u) = 0;

private:
	typedef std::map<std::string, DummyTrackedObject*> TrackedObjectList;
	TrackedObjectList	trackedObjects;
};
	
} /* wcl */


#endif /* end of include guard: DUMMYTRACKER_H_SWS8XTZ5 */


