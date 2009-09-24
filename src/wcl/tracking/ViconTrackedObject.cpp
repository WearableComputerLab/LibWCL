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
#include <iostream>
#include "ViconTrackedObject.h"
#include "ViconClient.h"


using namespace wcl;
using namespace std;

ViconTrackedObject::ViconTrackedObject(std::string name, ObjectType type)
{
	this->name = name;
	this->type = type;
	
	x = 0;
	y = 0;
	z = 0;
	rx = 0;
	ry = 0;
	rz = 0;
	units = Tracker::MM;
}

ViconTrackedObject::~ViconTrackedObject()
{
}

SMatrix ViconTrackedObject::getTransform()
{
	/*
	 *
	 * Construct Rotation Matrix
	 *
	 */
	double theta;
	double c, s, x, y, z;
	SMatrix M(4);
	theta = sqrt( rx*rx + ry*ry + rz*rz );

	if (theta < 1e-15)
	{
		M[0][0] = M[1][1] = M[2][2] = M[3][3] = 1.0;
		M[0][1] = M[0][2] = M[0][3] = M[1][0] = M[1][2] = M[1][3] = M[2][0] = M[2][1] = M[2][3] = M[3][0] = M[3][1] = M[3][2] = 0.0;
	}
	else
	{
		x = rx/theta;
		y = ry/theta;
		z = rz/theta;

		c = cos(theta);
		s = sin(theta);

		M[0][0] = c + (1-c)*x*x;
		M[1][0] =     (1-c)*x*y + s*(-z);
		M[2][0] =     (1-c)*x*z + s*y;
		M[0][1] =     (1-c)*y*x + s*z;
		M[1][1] = c + (1-c)*y*y;
		M[2][1] =     (1-c)*y*z + s*(-x);
		M[0][2] =     (1-c)*z*x + s*(-y);
		M[1][2] =     (1-c)*z*y + s*x;
		M[2][2] = c + (1-c)*z*z;

		//make it a 4x4 matrix
		M[3][0] = 0;
		M[3][1] = 0;
		M[3][2] = 0;
		M[0][3] = 0;
		M[1][3] = 0;
		M[2][3] = 0;
		M[3][3] = 1;
	}

	M = inv(M);


	/*
	 *
	 * Construct Translation Matrix
	 *
	 */
	SMatrix T(4);
	T[0][0] = 1;
	T[1][1] = 1;
	T[2][2] = 1;
	T[3][3] = 1;

	switch (this->units)
	{
		case Tracker::MM:
			T[0][3] = this->x;
			T[1][3] = this->y;
			T[2][3] = this->z;
			break;
		case Tracker::CM:
			T[0][3] = this->x/10.0;
			T[1][3] = this->y/10.0;
			T[2][3] = this->z/10.0;
			break;
		case Tracker::INCHES:
			T[0][3] = this->x/25.4;
			T[1][3] = this->y/25.4;
			T[2][3] = this->z/25.4;
			break;
	}

	return M*T;
}


Vector ViconTrackedObject::getTranslation()
{
	Vector v(3);
	switch (this->units)
	{
		case Tracker::MM:
			v[0] = x;
			v[1] = y;
			v[2] = z;
			break;
		case Tracker::CM:
			v[0] = x/10.0;
			v[1] = y/10.0;
			v[2] = z/10.0;
			break;
		case Tracker::INCHES:
			v[0] = x/25.4;
			v[1] = y/25.4;
			v[2] = z/25.4;
			break;
	}

	return v;
}


SMatrix ViconTrackedObject::getRotation()
{
	/*
	 *
	 * Construct Rotation Matrix
	 *
	 */
	double theta;
	double c, s, x, y, z;
	SMatrix M(4);
	theta = sqrt( rx*rx + ry*ry + rz*rz );

	if (theta < 1e-15)
	{
		M[0][0] = M[1][1] = M[2][2] = M[3][3] = 1.0;
		M[0][1] = M[0][2] = M[0][3] = M[1][0] = M[1][2] = M[1][3] = M[2][0] = M[2][1] = M[2][3] = M[3][0] = M[3][1] = M[3][2] = 0.0;
	}
	else
	{
		x = rx/theta;
		y = ry/theta;
		z = rz/theta;

		c = cos(theta);
		s = sin(theta);

		M[0][0] = c + (1-c)*x*x;
		M[0][1] =     (1-c)*x*y + s*(-z);
		M[0][2] =     (1-c)*x*z + s*y;
		M[1][0] =     (1-c)*y*x + s*z;
		M[1][1] = c + (1-c)*y*y;
		M[1][2] =     (1-c)*y*z + s*(-x);
		M[2][0] =     (1-c)*z*x + s*(-y);
		M[2][1] =     (1-c)*z*y + s*x;
		M[2][2] = c + (1-c)*z*z;

		//make it a 4x4 matrix
		M[0][3] = 0;
		M[1][3] = 0;
		M[2][3] = 0;
		M[3][0] = 0;
		M[3][1] = 0;
		M[3][2] = 0;
		M[3][3] = 1;
	}

	M = inv(M);
	return M;
}


std::string ViconTrackedObject::toString() {
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

void ViconTrackedObject::setUnits(Tracker::Units u)
{
	this->units = u;
}

void ViconTrackedObject::updateData(double* array, int &offset)
{
	//Six dof objects also have rotation
	if (this->type == SIX_DOF)
	{
		#ifdef WORDS_BIGENDIAN
		rx = ViconClient::reverseBytesDouble(array[offset++]);
		ry = ViconClient::reverseBytesDouble(array[offset++]);
		rz = array[offset++];
		#else
		rx = array[offset++];
		ry = array[offset++];
		rz = array[offset++];
		#endif

	}
	
	//Translation is common to both types
	#ifdef WORDS_BIGENDIAN
	x = ViconClient::reverseBytesDouble(array[offset++]);
	y = ViconClient::reverseBytesDouble(array[offset++]);
	z = array[offset++];
	#else
	x = array[offset++];
	y = array[offset++];
	z = array[offset++];
	#endif

	//The marker has an occluded value
	if (this->type == POSITION)
	{
		double o = array[offset++];
		if (o != 0)
			occluded = true;
		else
			occluded = false;
	}

}

