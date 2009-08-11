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
#include <math.h>

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


	double Face::getArea()
	{
		wcl::Vector p1 = v1->position;
		wcl::Vector p2 = v2->position;
		wcl::Vector p3 = v3->position;

		wcl::Vector xy(p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]);
		wcl::Vector xz(p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2]);

		double a = fabs((p1 - p2).normal());
		double c = fabs((p1 - p3).normal());
		double b = xy.angle(xz);

		return (a * c * sin(b)) / 2.0;
	}


	void Face::invert()
	{
		Vertex* temp = v2;
		v2 = v1;
		v1 = temp;
	}

	wcl::Vector Face::getNormal() const
	{
		wcl::Vector& vec1 = v1->position;
		wcl::Vector& vec2 = v2->position;
		wcl::Vector& vec3 = v3->position;

		return (vec2 - vec1).crossProduct(vec3 - vec1).unit();
	}

	Plane Face::getPlane() const
	{
		return Plane(v1->position,v2->position,v3->position);
	}

	bool Face::operator==(const Face& f) const
	{
		return (*v1 == *(f.v1) && *v2 == *(f.v2) && *v3 == *(f.v3));
	}

	bool Face::hasPoint(const wcl::Vector& p) const
	{
		LinePosition res1, res2, res3;
		bool hasUp, hasDown, hasOn;
		wcl::Vector normal = this->getNormal();

		if (fabs(normal[0]) > TOL)
		{
			res1 = linePositionInX(p, v1->position, v2->position);
			res2 = linePositionInX(p, v2->position, v3->position);
			res3 = linePositionInX(p, v3->position, v1->position);
		}
		else if (fabs(normal[1]) > TOL)
		{
			res1 = linePositionInY(p, v1->position, v2->position);
			res2 = linePositionInY(p, v2->position, v3->position);
			res3 = linePositionInY(p, v3->position, v1->position);
		}
		else
		{
			res1 = linePositionInY(p, v1->position, v2->position);
			res2 = linePositionInY(p, v2->position, v3->position);
			res3 = linePositionInY(p, v3->position, v1->position);
		}

		if (((res1 == UP)||(res2==UP)||(res3==UP)) && ((res1==DOWN)||(res2==DOWN)||(res3==DOWN)))
			return true;
		else if ((res1==ON)||(res2==ON)||(res3==ON))
			return true;
		else
			return false;

	}

	Face::LinePosition Face::linePositionInX(const wcl::Vector& point, const wcl::Vector& pLine1, const wcl::Vector& pLine2) const
	{
		double a,b,z;
		if ((fabs(pLine1[1] - pLine2[1]) > TOL) &&(((point[1]>=pLine1[1]) && (point[1] <= pLine2[1])) || ((point[1] <= pLine1[1]) && (point[1]>=pLine2[1]))))
		{
			a = (pLine2[2] - pLine1[2]) / (pLine2[1] - pLine1[1]);
			b = pLine1[2] - a*pLine1[1];
			z = a*point[1] + b;
			if (z> point[2] + TOL)
			{
				return UP;
			}
			else if (z < point[2] - TOL)
			{
				return DOWN;
			}
			else
			{
				return ON;
			}
		}
		else
		{
			return NONE;
		}
	}

	Face::LinePosition Face::linePositionInY(const wcl::Vector& point, const wcl::Vector& pLine1, const wcl::Vector& pLine2) const
	{
		double a,b,z;
		if ((fabs(pLine1[0] - pLine2[0]) > TOL) &&(((point[0]>=pLine1[0]) && (point[0] <= pLine2[0])) || ((point[0] <= pLine1[0]) && (point[0]>=pLine2[0]))))
		{
			a = (pLine2[2] - pLine1[2]) / (pLine2[0] - pLine1[0]);
			b = pLine1[2] - a*pLine1[0];
			z = a*point[0] + b;
			if (z> point[2] + TOL)
			{
				return UP;
			}
			else if (z < point[2] - TOL)
			{
				return DOWN;
			}
			else
			{
				return ON;
			}
		}
		else
		{
			return NONE;
		}
	}

	Face::LinePosition Face::linePositionInZ(const wcl::Vector& point, const wcl::Vector& pLine1, const wcl::Vector& pLine2) const
	{
		double a,b,y;
		if ((fabs(pLine1[0] - pLine2[0]) > TOL) &&(((point[0]>=pLine1[0]) && (point[0] <= pLine2[0])) || ((point[0] <= pLine1[0]) && (point[0]>=pLine2[0]))))
		{
			a = (pLine2[1] - pLine1[1]) / (pLine2[0] - pLine1[0]);
			b = pLine1[1] - a*pLine1[0];
			y = a*point[0] + b;
			if (y> point[1] + TOL)
			{
				return UP;
			}
			else if (y < point[1] - TOL)
			{
				return DOWN;
			}
			else
			{
				return ON;
			}
		}
		else
		{
			return NONE;
		}
	}
}

