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

#include <config.h>
#include <cstdlib>
#include <math.h>

#include "Plane.h"

namespace wcl
{

	Plane::Plane(const wcl::Vector& v1, const wcl::Vector& v2, const wcl::Vector& v3)
	{
		point = v1;
		normal = (v2 - v1).crossProduct(v3 - v1).unit();

		d = -(normal[0]*point[0] + normal[1]*point[1] + normal[2]*point[2]);
	}

	Plane::Plane()
	{
		d = 0;
	}

	double Plane::distanceFrom(const wcl::Vector& p) const
	{
		return (normal[0]*p[0] + normal[1]*p[1] + normal[2]*p[2] + d);
	}

	Line Plane::intersect(const Plane& p) const
	{
		//the direction is perpendicular to the two planes, or
		//the cross product
		wcl::Vector dir = this->normal.crossProduct(p.normal);
		wcl::Vector pos(3);
		
		//make it a unit vector
		dir = dir.unit();

		//find a point on the line..
		double d1 = -(this->normal[0]*this->point[0] +
					  this->normal[1]*this->point[1] +
					  this->normal[2]*this->point[2]);

		double d2 = -(p.normal[0]*p.point[0] +
					  p.normal[1]*p.point[1] +
					  p.normal[2]*p.point[2]);

		if (abs(dir[0]) > TOL)
		{
			pos[0] = 0;
			pos[1] = (d2*this->normal[2] - d1*p.normal[2])/dir[0];
			pos[2] = (d1*p.normal[1] - d2*normal[1])/dir[0];
		}
		else if (abs(dir[1]) > TOL)
		{
			pos[0] = (d1*p.normal[2] - d2*normal[2]) / dir[1];
			pos[1] = 0;
			pos[2] = (d2*normal[0] - d1*p.normal[0]) / dir[1];
		}
		else
		{
			pos[0] = (d2*normal[1] - d1*p.normal[1])/dir[2];
			pos[1] = (d1*p.normal[0] - d2*normal[0])/dir[2];
			pos[2] = 0;
		}
		return Line(pos, dir);
	}

	HNF Plane::toHNF() const
	{
		HNF hnf;
		double rootA2B2C2 = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);

		hnf.n[0] = normal[0] / rootA2B2C2;
		hnf.n[1] = normal[1] / rootA2B2C2;
		hnf.n[2] = normal[2] / rootA2B2C2;

		hnf.p = d / rootA2B2C2;

		return hnf;
	}

	wcl::Vector Plane::getNormal() const
	{
		return normal;
	}

	double Plane::getD() const
	{
		return d;
	}

	wcl::Vector Plane::getPosition() const
	{
		return normal;
	}

	std::string Plane::toString()
	{
		std::stringstream ss;
		ss << "Plane. Normal: (" << normal[0] << ", " << normal[1] << ", " << normal[2] << "), d:" << d << std::endl;
		return ss.str();
	}
}

