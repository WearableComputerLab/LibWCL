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

#include <wcl/tracking/PolhemusTrackedObject.h>

namespace wcl
{
	PolhemusTrackedObject::PolhemusTrackedObject() 
		: position(3)
	{
		//all of the polhemus trackers are 6dof
		type = SIX_DOF;
		confidence = 1.0f;
	}

	PolhemusTrackedObject::~PolhemusTrackedObject()
	{}

	std::string PolhemusTrackedObject::toString() const
	{
		std::stringstream ss;
		ss << "Position: " << position[0] << " " << position[1] << " " << position[2] << " ";
		return ss.str();
	}

	SMatrix PolhemusTrackedObject::getTransform() const
	{
		SMatrix T(4);
		T[0][0] = 1;
		T[1][1] = 1;
		T[2][2] = 1;
		T[3][3] = 1;

		T[0][3] = position[0];
		T[1][3] = position[1];
		T[2][3] = position[2];

		return T * getOrientation().getRotation();
	}

	Vector PolhemusTrackedObject::getTranslation() const
	{
		return position;
	}

	Quaternion PolhemusTrackedObject::getOrientation() const
	{
		return orientation;
	}

	void PolhemusTrackedObject::update(T x, T y, T z, T rw, T rx, T ry, T rz)
	{
		position[0] = x;
		position[1] = y;
		position[2] = z;

		orientation.set(rw, rx, ry, rz);
	}

	bool PolhemusTrackedObject::isVisible() const
	{
		//TODO Does the polhemus make it possible to actually check for visibility?
		return true;
	}
}

