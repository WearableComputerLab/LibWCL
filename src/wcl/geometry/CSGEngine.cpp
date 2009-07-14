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


#include "CSGEngine.h"


namespace wcl
{


	CSGEngine::CSGEngine()
	{}


	CSGEngine::~CSGEngine()
	{}


	PolygonObject* CSGEngine::csgUnion(const PolygonObject& objectA, const PolygonObject& objectB)
	{
		//TODO Make sure the objects are valid
		
		// make copies of the objects
		PolygonObject a(objectA);
		PolygonObject b(objectB);

		//subdivide
		split(a, b);
		split(b, a);
		split(a, b);

	
	}


	PolygonObject* CSGEngine::csgIntersection(const PolygonObject& objectA, const PolygonObject& objectB)
	{}


	PolygonObject* CSGEngine::csgDifference(const PolygonObject& objectA, const PolygonObject& objectB)
	{}

	void CSGEngine::split(PolygonObject& objectA, PolygonObject& objectB)
	{
	} // function


	CSGEngine::IntersectionType CSGEngine::intersect(Face& objectA, Face& objectB)
	{
		//find signed distance from each vertex in objectA to the plane of objectB
		//if all distances are zero, coplanar
		//if all positive or all negative, no intersection
		bool allZero = true;
		bool allPositive = true;
		bool allNegative = true;

		std::vector<wcl::Vertex*>::const_iterator it;

		wcl::Plane bPlane = objectB.getPlane();

		for (it = objectA.getVerts().begin(); it < objectA.getVerts().end(); ++it)
		{
			if (bPlane.distanceFrom((*it)->position) < 0)
			{
				allPositive = false;
				allZero = false;
			}
			else if (bPlane.distanceFrom((*it)->position) > 0)
			{
				allNegative = false;
				allZero = false;
			}
			else
			{
				allPositive = false;
				allNegative = false;
			}
		}
		if (allZero)
		{
			return COPLANAR;
		}
		if (allPositive || allNegative)
		{
			return NO_INTERSECT;
		}

		//find signed distance from each vertex in objectB to the plane of objectA
		//if all distances are positive or all negative, no intersection
		allPositive = true;
		allNegative = true;

		wcl::Plane aPlane = objectA.getPlane();

		for (it = objectB.getVerts().begin(); it < objectB.getVerts().end(); ++it)
		{
			if (aPlane.distanceFrom((*it)->position) < 0)
			{
				allPositive = false;
				allZero = false;
			}
			else if (aPlane.distanceFrom((*it)->position) > 0)
			{
				allNegative = false;
				allZero = false;
			}
			else
			{
				allPositive = false;
				allNegative = false;
			}
		}
		if (allPositive || allNegative)
		{
			return NO_INTERSECT;
		}
		//else
		//calculate line of intersection of the two planes
		wcl::Line pi = aPlane.intersect(bPlane);

		//calculate line segments that pass through objectA and objectB
		//if they overlap, there is an intersection
		return NO_INTERSECT;
	}
};

