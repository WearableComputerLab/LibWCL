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


#ifndef POLYGON_OBJECT_H
#define POLYGON_OBJECT_H

#include <string>
#include <vector>

#include <wcl/api.h>
#include <wcl/Exception.h>
#include <wcl/maths/Vector.h>
#include <wcl/geometry/BoundingBox.h>
#include <wcl/geometry/Face.h>
#include <wcl/geometry/LineSegment.h>
#include <wcl/geometry/Vertex.h>

namespace wcl
{
	/**
	 * Representation of a polygonal object made of 1 or more polygons.
	 */
	class WCL_API PolygonObject
	{
		public:
			/**
			 * Default Constructor. Creates an object with zero polygons
			 */
			PolygonObject();

			/**
			 * Copy Constructor.
			 * Makes this and object the same by performing a deep copy
			 * of all members.
			 */
			PolygonObject(const PolygonObject& object);

			/**
			 * Overloaded = operator, performs a deep copy of object.
			 */
			const PolygonObject& operator=(const PolygonObject& object);

			/**
			 * Returns the list of faces for this object.
			 * Be a bit careful here.
			 */
			std::vector<Face*>& getFaces();

			/**
			 * Destructor.
			 */
			~PolygonObject();

			/**
			 * Returns the axis aligned bounding box of this object.
			 */
			wcl::BoundingBox getBoundingBox() const;

			/**
			 * Adds a vertex to the list, if an equal vertex doesn't already
			 * exist.
			 *
			 * @returns the vertex
			 */
			Vertex* addVertex(const wcl::Vector& position);

			/**
			 * Adds a triangular face to the polygon. A number of checks
			 * are performed to confirm that face is actually valid.
			 * These are:
			 *	1. No two vertexes are the same
			 *	2. The face area is greater than the tolerance specified by TOL
			 * If all checks are valid the face is stored in the
			 * internal facelist and also returned.
			 *
			 * @param v1 Vertex 1 of the face
			 * @param v2 Vertex 2 of the face
			 * @param v3 Vertex 3 of the face
			 * @return A pointer to the face
			 * @throws Exception if checks are not met
			 */
			Face* addFace(Vertex* v1, Vertex* v2, Vertex* v3) throw(Exception);

		protected:
			std::vector<Face*> faceList;

		private:
			std::vector<wcl::Vertex*> vertexList;
	};
}

#endif

