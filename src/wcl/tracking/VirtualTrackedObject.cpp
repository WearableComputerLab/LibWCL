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


#include <wcl/tracking/VirtualTrackedObject.h>

namespace wcl
{

	VirtualTrackedObject::VirtualTrackedObject(std::string _name)
		: orientation(), translation(3)
	{
		name = _name;
		type = SIX_DOF;
	}


	std::string VirtualTrackedObject::toString() const
	{
		//TODO implement me
		return "VirtualTrackedObject";
	}

	SMatrix VirtualTrackedObject::getTransform() const
	{
		SMatrix T(4);
		T[0][0] = 1;
		T[1][1] = 1;
		T[2][2] = 1;
		T[3][3] = 1;

		T[0][3] = translation[0];
		T[1][3] = translation[1];
		T[2][3] = translation[2];

		return T * getOrientation().getRotation();

	}

	Vector VirtualTrackedObject::getTranslation() const
	{
		return translation;
	}

	void VirtualTrackedObject::setData(const double& x,
							           const double& y,
							           const double& z,
							           const double& rw,
							           const double& rx,
							           const double& ry,
							           const double& rz)
	{
		this->translation[0] = x;
		this->translation[1] = y;
		this->translation[2] = z;
		this->orientation.set(rw,rx,ry,rz);
	}


	wcl::Quaternion VirtualTrackedObject::getOrientation() const
	{
		return orientation;
	}

	bool VirtualTrackedObject::isVisible() const
	{
		return true;
	}
}

