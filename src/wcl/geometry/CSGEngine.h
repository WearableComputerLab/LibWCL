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


#ifndef CSG_ENGINE_H
#define CSG_ENGINE_H

#include <wcl/geometry/PolygonObject.h>

namespace wcl
{
	/**
	 * A class that is able to perform CSG operations on polygon object.
	 */
	class CSGEngine
	{

		public:
			enum IntersectionType
			{
				COPLANAR,
				NO_INTERSECT,
				INTERSECT
			};


			/**
			 * Default constructor.
			 */
			CSGEngine();

			/**
			 * Destructor.
			 */
			~CSGEngine();

			/**
			 * Performs a Union operation on two objects.
			 * return = objectA U objectB
			 *
			 * Note here that the parameters are not modified at all.
			 * YOU are responsible for deleting the returned object when it
			 * is nolonger required.
			 * 
			 * @param objectA A polygon object to use.
			 * @param objectB A polygon object to use.
			 * @return Returns a pointer to a new Polygon Object that is the result of the operation.
			 */
			PolygonObject* csgUnion(const PolygonObject& objectA, const PolygonObject& objectb);


			/**
			 * Performs an Intersectionoperation on two objects.
			 * return = objectA n objectB
			 *
			 * Note here that the parameters are not modified at all.
			 * YOU are responsible for deleting the returned object when it
			 * is nolonger required.
			 * 
			 * @param objectA A polygon object to use.
			 * @param objectB A polygon object to use.
			 * @return Returns a pointer to a new Polygon Object that is the result of the operation.
			 */
			PolygonObject* csgIntersection(const PolygonObject& objectA, const PolygonObject& objectb);


			/**
			 * Performs a Difference operation on two objects.
			 * return = objectA - objectB
			 *
			 * Note here that the parameters are not modified at all.
			 * YOU are responsible for deleting the returned object when it
			 * is nolonger required.
			 * 
			 * @param objectA A polygon object to use.
			 * @param objectB A polygon object to use.
			 * @return Returns a pointer to a new Polygon Object that is the result of the operation.
			 */
			PolygonObject* csgDifference(const PolygonObject& objectA, const PolygonObject& objectb);

		private:
			/**
			 * Splits objectA by objectB.
			 * ObjectB is not modified, A is.
			 */
			void split(PolygonObject& objectA, PolygonObject& objectB);

			IntersectionType intersect(Face& objectA, Face& objectB);


	};
};

#endif

