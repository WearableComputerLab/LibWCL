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


#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <vector>

#include <wcl/geometry/LineSegment.h>
#include <wcl/geometry/Vertex.h>
#include <wcl/maths/Vector.h>
#include <config.h>

namespace wcl
{
	/**
	 * An axis aligned bounding box for some object.
	 */
	class BoundingBox
	{
		public:
			/**
			 * Creates a bounding box with the extents as passed
			 * in parameters.
			 *
			 * @param min The minimum x, y and z.
			 * @param max The minimum x, y and z.
			 */
			BoundingBox(const wcl::Vector& min, const wcl::Vector& max);

			/**
			 * Creates a bounding box from a list of vertices.
			 *
			 * @param verts The list of vertices to use for the bounding box.
			 */
			BoundingBox(const std::vector<Vertex*>& verts);

			/**
			 * Creates a bounding box with invalid parameters so
			 * that addPoint can be used.
			 *
			 * min = MAX_DOUBLE
			 * max = MIN_DOUBLE
			 */
			BoundingBox();

			/**
			 * Returns true if this bounding box overlaps with b
			 *
			 * @param b The bounding box to test against
			 */
			bool overlaps(const wcl::BoundingBox& b) const;

			bool intersect(const wcl::LineSegment& s) const;

			/**
			 * Adds a point to the bounding box, adjusting
			 * the size if needed.
			 *
			 * @param p The point to add to the bounding volume.
			 */
			void addPoint(const wcl::Vector& p);

			/**
			 * Resets the bounding box to an invalid state
			 */
			void clear();

			/**
			 * Returns the centroid of the bounding box.
			 */
			wcl::Vector getCentroid();

		private:
			/**
			 * the minimum x,y and z
			 */
			wcl::Vector min;

			/**
			 * the maximum x,y and z
			 */
			wcl::Vector max;
	};
}

#endif

