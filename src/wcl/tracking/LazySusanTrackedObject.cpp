/*-
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


#include "LazySusanTrackedObject.h"

namespace wcl
{

	LazySusanTrackedObject::LazySusanTrackedObject() :  mRotation(0)
	{
		name = "LazySusan";
	}


	LazySusanTrackedObject::~LazySusanTrackedObject()
	{}


	std::string LazySusanTrackedObject::toString() const
	{
		return name;
	}

	SMatrix LazySusanTrackedObject::getTransform() const
	{
		return mOrientation.getRotation();
	}

	Vector LazySusanTrackedObject::getTranslation() const
	{
		return wcl::Vector(0,0,0);
	}

	Quaternion LazySusanTrackedObject::getOrientation() const
	{
		return mOrientation;
	}
	
	double LazySusanTrackedObject::getRotation() const
	{
		return mRotation;
	}

	bool LazySusanTrackedObject::isVisible() const
	{
		return true;
	}

	void LazySusanTrackedObject::setRotation(double rotation)
	{
		//in degrees...
		rotation *= 360;
	
		mRotation = rotation;
		// to radians
		static double pi = 3.14159265;
		double radian = rotation * (pi/180);

		mOrientation = wcl::Quaternion(wcl::Vector(0,1,0), radian);
	}
}
