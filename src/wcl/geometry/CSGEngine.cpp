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
		//if extent of objectA overlaps extent of objectB
		if (objectA.getBoundingBox().overlaps(objectB.getBoundingBox()))
		{
			//foreach polygonA in objectA
			std::vector<wcl::Face*> faces = objectA.getFaces();
			std::vector<wcl::Face*>::iterator polygonA;

			for(polygonA = faces.begin(); polygonA < faces.end(); ++polygonA)
			{
				//if the extent of polygonA overlaps extent of objectB
				if (objectB.getBoundingBox().overlaps((*polygonA)->getBoundingBox()))
				{
					//foreach polygonB in objectB
					std::vector<wcl::Face*> bFaces = objectB.getFaces();
					std::vector<wcl::Face*>::const_iterator polygonB;

					for(polygonB = bFaces.begin(); polygonB < bFaces.end(); ++polygonB)
					{
						//if the extents of polygonA and polygonB overlap
						if ((*polygonA)->getBoundingBox().overlaps((*polygonB)->getBoundingBox()))
						{
							//do they intersect
							//if they are not coplanar and do intersect
							if (intersect((*(*polygonA)), (*(*polygonB))) == INTERSECT)
							{
								//subdivide polygonA with polygonB
							}
						} // what
					} // a
				} // big
			} //ugly
		} // nesting
	} // function


	CSGEngine::IntersectionType CSGEngine::intersect(Face& objectA, Face& objectB)
	{
		return NO_INTERSECT;
	}
};

