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


#ifndef LINE_SEGMENT_H
#define LINE_SEGMENT_H

#include <string>

#include <config.h>

#include <wcl/maths/Vector.h>
#include <wcl/geometry/Face.h>
#include <wcl/geometry/Line.h>

class Face;

namespace wcl
{
	class LineSegment
	{
		public:
			enum LineIntersectType
			{
				VERTEX,
				FACE,
				EDGE,
				UNKNOWN //should never be this!
			};

			LineSegment(const Line& line, const Face& face, int sign1, int sign2, int sign3);

			bool intersect(const LineSegment& s);

			double startDistance;
			double endDistance;
			int index;

			LineIntersectType startType;
			LineIntersectType middleType;
			LineIntersectType endType;

			Line line;
			Vertex* startVert;
			Vertex* endVert;

			wcl::Vector startPos;
			wcl::Vector endPos;

			std::string toString();

		private:
			bool setVertex(Vertex* v);
			bool setEdge(Vertex* v1, Vertex* v2);
			void swapEnds();
	};
}

#endif

