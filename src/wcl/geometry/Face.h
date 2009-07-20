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


#ifndef FACE_H
#define FACE_H

#include <wcl/geometry/BoundingBox.h>
#include <wcl/geometry/IntersectStatus.h>
#include <wcl/geometry/Plane.h>
#include <wcl/geometry/Vertex.h>
#include <config.h>

class PolygonObject;

namespace wcl
{
	/**
	 * A single face of a polygon.
	 */
	class Face 
	{
		public:

			/**
			 * Copy Constructor.
			 */
			Face (const Face& f);

			Face (Vertex* v1,Vertex* v2,Vertex* v3);

			/**
			 * Returns the axis aligned bounding box of this Face.
			 */
			const BoundingBox& getBoundingBox();

			/**
			 * Returns the surface normal of this face.
			 */
			wcl::Vector getNormal();

			/**
			 * Returns a plane representing this face.
			 * Note that this only works with planar faces (so maybe not N-gons)
			 */
			Plane getPlane();

			/**
			 * Adds a vertex to the face.
			 * THE FACE DOES NOT DELETE ANY VERTICES
			 * because they could also be used in other faces.
			 */
			void addVertex(Vertex* v);

			const std::vector<Vertex*>& getVerts();

			bool operator== (const Face& f) const;

			bool quickClassify();

			double getArea();

			/**
			 * The vertices
			 */
			Vertex* v1;
			Vertex* v2;
			Vertex* v3;
		private:

			IntersectStatus status;

			/**
			 * Calculating a bounding box whenever it's needed is slow,
			 * so we'll just maintain one and return a reference to it when needed.
			 */
			BoundingBox boundingBox;
	};
};


#endif

