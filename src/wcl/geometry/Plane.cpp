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

#include "Plane.h"

namespace wcl
{

	Plane::Plane(const wcl::Vector& v1, const wcl::Vector& v2, const wcl::Vector& v3)
	{
		p = v1;
		normal = (v2 - v1).crossProduct(v3 - v1).unit();

		d = -v1[0]*(v2[1]*v3[2] - v3[1]*v2[2]) 
			- v2[0]*(v3[1]*v1[2] - v1[1]*v3[2]) 
			- v3[0]*(v1[1]*v2[2] - v2[1]*v1[2]);
	}

	double Plane::distanceFrom(const wcl::Vector& p)
	{
		return (normal[0]*p[0] + normal[1]*p[1] + normal[2]*p[2] + d) / 
			   sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
	}

	PlaneIntersection Plane::intersect(const Plane& p)
	{
		PlaneIntersection p;
		//the direction is perpendicular to the two planes, or
		//the cross product
		p.dir = this->normal.crossProduct(p.normal);
		
		//make it a unit vector
		p.dir = p.dir.unit();

		//TODO fixme

		return p;
	}

}

