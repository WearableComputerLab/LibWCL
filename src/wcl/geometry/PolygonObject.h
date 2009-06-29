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

namespace wcl
{

	/**
	 * The status of a polygon, used for CSG operations.
	 */
	enum VertexStatus 
	{
		INSIDE,
		OUTSIDE,
		BOUNDARY,
		UNKNOWN
	};


	/**
	 * Structure of a single vertex.
	 */
	struct Vertex
	{
		/**
		 * The position of the vertex in 3D space
		 */
		wcl::Vector position;

		/**
		 * Vertices that are adjacent to this one.
		 */
		std::vector<Vertex*> adjacentVerts;
		
		/**
		 * Status of the vertex, used during CSG operations
		 */
		VertexStatus status;
	};
		
	struct Face 
	{
		std::vector<Vertex*> verts;
	};

	/**
	 * Representation of a polygonal object made of 1 or more polygons.
	 */
	class PolygonObject
	{
		public:
			/**
			 * Default Constructor. Creates an object with zero polygons
			 */
			PolygonObject(std::string id="");

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
			 * Destructor.
			 */
			~PolygonObject();


			/**
			 * Returns the minimum x,y and z for this object's
			 * bounding box.
			 */
			wcl::Vector getMinExtent() const;

			/**
			 * Returns the maximum x,y and z for this object's
			 * bounding box.
			 */
			wcl::Vector getMaxExtent() const;

		private:
			std::string id;
			std::vector<Face*> faceList;
			std::vector<wcl::Vector*> vertexList;
	};
};

#endif

