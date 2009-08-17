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

#include <wcl/maths/Vector.h>
#include <config.h>

#include <wcl/geometry/BoundingBox.h>
#include <wcl/geometry/Face.h>
#include <wcl/geometry/IntersectStatus.h>
#include <wcl/geometry/LineSegment.h>
#include <wcl/geometry/Vertex.h>

namespace wcl
{
	/**
	 * Representation of a polygonal object made of 1 or more polygons.
	 */
	class PolygonObject
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
			 * Returns a new PolygonObject consisting of a union of this
			 * and b.
			 *
			 * @param b The object to union with this.
			 * @return A new polygon object representing the result of the union.
			 */
			PolygonObject* csgUnion(const PolygonObject& b);

			/**
			 * Returns a new PolygonObject consisting of an intersect of this
			 * and b.
			 *
			 * @param b The object to union with this.
			 * @return A new polygon object representing the result of the intersect.
			 */
			PolygonObject* csgIntersect(const PolygonObject& b);

			/**
			 * Returns a new PolygonObject consisting of a difference of this
			 * and b.
			 *
			 * result = this - b
			 *
			 * @param b The object to union with this.
			 * @return A new polygon object representing the result of the difference.
			 */
			PolygonObject* csgDifference(const PolygonObject& b);

			/**
			 * Adds a vertex to the list, if an equal vertex doesn't already
			 * exist.
			 *
			 * @returns the vertex
			 */
			Vertex* addVertex(const wcl::Vector& position, IntersectStatus v = UNKNOWN);

			Face* addFace(Vertex* v1, Vertex* v2, Vertex* v3);

		protected:
			std::vector<Face*> faceList;

		private:
			std::vector<wcl::Vertex*> vertexList;

			void classifyFaces(const PolygonObject& object);
			void raytraceClassify(Face& f) const;
			void splitFaces(const PolygonObject& obj);
			void splitFace(int index, const LineSegment& segment1, const LineSegment& segment2);
			void breakFaceInTwo(int facePos, wcl::Vector newPos, int splitEdge);
			void breakFaceInTwo(int facePos, wcl::Vector newPos, Vertex* endVertex);
			void breakFaceInThree(int facePos, wcl::Vector newPos1, wcl::Vector newPos2, int splitEdge);
			void breakFaceInThree(int facePos, wcl::Vector newPos, Vertex* endVertex);
			void breakFaceInThree(int facePos, const wcl::Vector& newPos1, const wcl::Vector& newPos2, Vertex* startVertex, Vertex* endVertex);
			void breakFaceInThree(int facePos, wcl::Vector newPos);
			void breakFaceInFour(int facePos, wcl::Vector newPos1, wcl::Vector newPos2, Vertex* endVertex);
			void breakFaceInFive(int facePos, wcl::Vector newPos1, wcl::Vector newPos2, int linedVertex);


			/**
			 * inverts the faces that have been classified as INSIDE.
			 * Needed when doing difference operation.
			 */
			void invertInsideFaces();
	};
}

#endif

