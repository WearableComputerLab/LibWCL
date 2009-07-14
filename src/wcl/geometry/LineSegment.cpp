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


#include <wcl/geometry/LineSegment.h>


namespace wcl
{
	LineSegment::LineSegment(const Line& l, const Face& face, int sign1, int sign2, int sign3) : line(l)
	{
		index = 0;

		if (sign1 == 0)
		{

		}
	}

	bool LineSegment::setVertex(Vertex* v)
	{
		if (index = 0)
		{
			startVert = v;
			startType = VERTEX;
			startDistance = line.distanceFromPoint(v->position);
			startPos = v->position;
			index++;
			return true;
		}
		else if (index == 1)
		{
			endVert = v;
			endType = VERTEX;
			endDistance = line.distanceFromPoint(v->position);
			endPos = v->position;
			index++;

			if (startVert == endVert)
			{
				middleType = VERTEX;
			}
			else if (startType == VERTEX)
			{
				middleType = EDGE;
			}

			if (startDistance > endDistance)
			{
				this->swapEnds();
			}
			return true;
		}
		return false;
	}

	void LineSegment::swapEnds()
	{
		double tempDist = startDistance;
		startDistance = endDistance;
		endDistance = tempDist;

		LineIntersectType tempType = startType;
		startType = endType;
		endType = tempType;

		Vertex* tempVert = startVert;
		startVert = endVert;
		endVert= tempVert;

		wcl::Vector tempPos = startPos;
		startPos = endPos;
		endPos = tempPos;
	}
}

