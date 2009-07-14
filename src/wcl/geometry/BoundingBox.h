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

#include <wcl/geometry/Vertex.h>
#include <wcl/maths/Vector.h>
#include <config.h>

namespace wcl
{
	class BoundingBox
	{
		public:
			/**
			 * Creates a bounding box with the extents as passed
			 * in parameters.
			 */
			BoundingBox(const wcl::Vector& min, const wcl::Vector& max);


			/**
			 * Creates a bounding box from a list of vertices.
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
			 */
			bool overlaps(const wcl::BoundingBox& b) const;

			/**
			 * Adds a point to the bounding box, adjusting
			 * the size if needed.
			 */
			void addPoint(const wcl::Vector& p);

		private:
			wcl::Vector min;
			wcl::Vector max;
	};
};

#endif

