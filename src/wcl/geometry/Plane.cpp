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
		point = v1;
		normal = (v2 - v1).crossProduct(v3 - v1).unit();

		d = -v1[0]*(v2[1]*v3[2] - v3[1]*v2[2]) 
			- v2[0]*(v3[1]*v1[2] - v1[1]*v3[2]) 
			- v3[0]*(v1[1]*v2[2] - v2[1]*v1[2]);
	}

	double Plane::distanceFrom(const wcl::Vector& p) const
	{
		return (normal[0]*p[0] + normal[1]*p[1] + normal[2]*p[2] + d) / 
			   sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
	}

	PlaneIntersection Plane::intersect(const Plane& p) const
	{
		PlaneIntersection i;
		//the direction is perpendicular to the two planes, or
		//the cross product
		i.dir = this->normal.crossProduct(p.normal);
		
		//make it a unit vector
		i.dir = i.dir.unit();

		//find a point on the line..

		HNF plane1 = this->toHNF();
		HNF plane2 = p.toHNF();

		//this only works if the direction.z is not 0
		if (i.dir[2] != 0)
		{
			double y = (plane2.n[0]*plane1.p - plane2.p/plane1.n[0]) / 
				(plane2.n[1]*plane1.n[0] - plane1.n[1]*plane2.n[0]);

			double x = (-plane1.p - plane1.n[1]*y) / plane1.n[0];

			i.p = wcl::Vector(x,y,0);
		}

		//otherwise, lets just swap with y
		else
		{
			double z = (plane2.n[0]*plane1.p - plane2.p/plane1.n[0]) / 
				(plane2.n[2]*plane1.n[0] - plane1.n[2]*plane2.n[0]);

			double x = (-plane1.p - plane1.n[z]*z) / plane1.n[0];

			i.p = wcl::Vector(x,0,z);
		}

		return i;
	}

	HNF Plane::toHNF() const
	{
		HNF hnf;
		double rootA2B2C2 = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);

		hnf.n[0] = normal[0] / rootA2B2C2;
		hnf.n[1] = normal[1] / rootA2B2C2;
		hnf.n[2] = normal[2] / rootA2B2C2;

		hnf.p = d / rootA2B2C2;
	}

}

