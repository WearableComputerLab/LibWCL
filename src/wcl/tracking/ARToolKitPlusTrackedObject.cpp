/*-
 * Copyright (c) 2010 Benjamin Close <Benjamin.Close@clearchain.com>
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
#include "ARToolKitPlusTrackedObject.h"


namespace wcl {


ARToolKitPlusTrackedObject::ARToolKitPlusTrackedObject(
    const unsigned w,
    const unsigned id):
    width(w),
    visible(false),
    seen(false),
    transform(4)
{
    std::stringstream sid;
    sid<< "Marker";
    sid<< id;
    this->name=sid.str();
    this->type=SIX_DOF;
    this->id =id;
}

ARToolKitPlusTrackedObject::~ARToolKitPlusTrackedObject()
{}

std::string ARToolKitPlusTrackedObject::toString() const
{
    std::stringstream ss;
    Vector position = this->getTranslation();
    ss << "Position: " << position[0] << " " << position[1] << " " << position[2] << " ";
    return ss.str();

}

SMatrix ARToolKitPlusTrackedObject::getTransform() const
{
    return this->transform;
}

Vector ARToolKitPlusTrackedObject::getTranslation() const
{
    Vector v(3);
    SMatrix s = this->getTransform();
    v[0]=s[0][3];
    v[1]=s[1][3];
    v[2]=s[2][3];
    return v;
}

Quaternion ARToolKitPlusTrackedObject::getOrientation() const
{
    SMatrix t = this->getTransform();

	//zero out the translation...
	t[0][3] = 0.0;
	t[1][3] = 0.0;
	t[2][3] = 0.0;

    return Quaternion(t);
}

unsigned ARToolKitPlusTrackedObject::getID() const
{
    return this->id;
}

void ARToolKitPlusTrackedObject::setTransform(const SMatrix &s )
{
    this->transform=s;
}

void ARToolKitPlusTrackedObject::setVisible ( const bool state )
{
    this->visible = state;
	if (visible)
		seen = true;
}

bool ARToolKitPlusTrackedObject::isVisible() const
{
    return this->visible;
}

bool ARToolKitPlusTrackedObject::hasBeenSeen() const
{
	return seen;
}

unsigned ARToolKitPlusTrackedObject::getWidth() const
{
    return this->width;
}


void ARToolKitPlusTrackedObject::setConfidence( const float value )
{
    this->confidence = value;
}

}
