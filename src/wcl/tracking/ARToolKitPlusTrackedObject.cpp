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
    const unsigned h,
    const unsigned id):
    width(w),
    height(h),
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

std::string ARToolKitPlusTrackedObject::toString()
{
    std::stringstream ss;
    Vector position = this->getTranslation();
    SMatrix orientation = this->getRotation();
    ss << "Position: " << position[0] << " " << position[1] << " " << position[2] << " ";
    //ss << orientation.toString();
    return ss.str();

}

SMatrix ARToolKitPlusTrackedObject::getTransform()
{
    return this->transform;
}

Vector ARToolKitPlusTrackedObject::getTranslation()
{
    SMatrix s = this->getTransform();
    return *s[2];
}

SMatrix ARToolKitPlusTrackedObject::getRotation()
{
    SMatrix t = this->getTransform();

    SMatrix s(3);
    for(unsigned r=0; r < 3; r++ ){
        for(unsigned c=0; c<3; c++)
            s[r][c]=t[r][c];
    }

    return t;
}

unsigned ARToolKitPlusTrackedObject::getID()
{
    return this->id;
}

void ARToolKitPlusTrackedObject::setTransform(const SMatrix &s )
{
    this->transform=s;
}

}
