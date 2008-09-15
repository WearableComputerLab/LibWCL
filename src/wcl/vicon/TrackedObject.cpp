/*-
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

#include <math.h>
#include <sstream>

#include "TrackedObject.h"


using namespace wcl;

TrackedObject::TrackedObject(std::string name, ObjectType type)
{
	this->name = name;
	this->type = type;
	
	x = 0;
	y = 0;
	z = 0;
	rx = 0;
	ry = 0;
	rz = 0;
	
}

TrackedObject::~TrackedObject()
{
}

std::string TrackedObject::getName()
{
	return name;
}

std::string TrackedObject::toString() {
	std::stringstream s;
	s << name;
	s << ": (" ;
	s << x ;
	s << ", "; 
	s << y; 
	s << ", ";
	s << z;
	s << ") (";
	s << rx << ", " << ry << ", " << rz << ")";
	return s.str(); 	
	
}

void TrackedObject::updateData(double* array, int &offset)
{
	//Translation is common to both types
	x = array[offset++];
	y = array[offset++];
	z = array[offset++];

	//The marker has an occluded value
	if (this->type == MARKER)
	{
		double o = array[offset++];
		if (o > 0)
			occluded = true;
		else
			occluded = false;
	}

	//Six dof objects also have rotation
	else if (this->type == SIX_DOF)
	{
		x = array[offset++];
		y = array[offset++];
		z = array[offset++];

		rx = array[offset++];
		ry = array[offset++];
		rz = array[offset++];
	}
}


