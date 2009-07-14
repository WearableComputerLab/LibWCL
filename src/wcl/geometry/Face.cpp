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


#include <assert.h>

#include "Face.h"
#include "Plane.h"

namespace wcl
{

	Face::Face(const Face& f)
	{
		//let std::vector do the copying for us
		//since we are only storing pointers after all...
		this->v1 = f.v1;
		this->v2 = f.v2;
		this->v3 = f.v3;
	}

	Face::Face(Vertex* v1,Vertex* v2,Vertex* v3)
	{
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}

	const BoundingBox& Face::getBoundingBox()
	{
		return boundingBox;
	}


	wcl::Vector Face::getNormal()
	{
		wcl::Vector& vec1 = v1->position;
		wcl::Vector& vec2 = v2->position;
		wcl::Vector& vec3 = v3->position;

		return (vec2 - vec1).crossProduct(vec3 - vec1).unit();
	}

	Plane Face::getPlane()
	{
		return Plane(v1->position,v2->position,v3->position);
	}


}

