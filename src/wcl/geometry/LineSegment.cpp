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


#include <sstream>
#include <iostream>

#include <wcl/geometry/LineSegment.h>
#include <wcl/geometry/Line.h>


namespace wcl
{
	LineSegment::LineSegment(const Line& l, const Face& face, int sign1, int sign2, int sign3) : 
		line(l), startPos(3), endPos(3), startType(UNKNOWN), middleType(UNKNOWN), endType(UNKNOWN)
	{
		index = 0;

		//first 3 conditions are for VERT-VERT-VERT
		if (sign1 == 0)
		{
			setVertex(face.v1);
			if (sign2 == sign3)
			{
				setVertex(face.v1);
			}
		}

		if (sign2 == 0)
		{
			setVertex(face.v2);
			if (sign1 == sign3)
			{
				setVertex(face.v2);
			}
		}

		if (sign3 == 0)
		{
			setVertex(face.v3);
			if (sign1 == sign2)
			{
				setVertex(face.v3);
			}
		}

		if (index != 2)
		{
			if ((sign1==1 && sign2==-1) || (sign1==-1 && sign2 == 1))
			{
				setEdge(face.v1, face.v2);
			}
			if ((sign2==1 && sign3==-1) || (sign2==-1 && sign3==1))
			{
				setEdge(face.v2, face.v3);
			}
			if ((sign3==1 && sign1==-1) || (sign3==-1 && sign1==1))
			{
				setEdge(face.v3, face.v1);
			}
		}
	}

	bool LineSegment::setVertex(Vertex* v)
	{
		if (index == 0)
		{
			startVert = v;
			startType = VERTEX;
			startDistance = line.distanceFromPoint(v->position);
			startPos = v->position;
			index++;
			return true;
		}
		if (index == 1)
		{
			endVert = v;
			endType = VERTEX;
			endDistance = line.distanceFromPoint(v->position);
			endPos = v->position;
			index++;

			if (*startVert == *endVert)
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

	bool LineSegment::setEdge(Vertex* v1, Vertex* v2)
	{
		wcl::Vector p1 = v1->position;
		wcl::Vector p2 = v2->position;
		wcl::Vector edgeDir(p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]);
		edgeDir = edgeDir.unit();
		Line edgeLine(edgeDir, p1);

		if (index == 0)
		{
			startVert = v1;
			startType = EDGE;
			startPos = line.intersect(edgeLine);
			startDistance = line.distanceFromPoint(startPos);
			middleType = FACE;
			index++;
			return true;
		}
		else if (index == 1)
		{
			endVert = v1;
			endType = EDGE;
			endPos = line.intersect(edgeLine);
			endDistance = line.distanceFromPoint(endPos);
			middleType  = FACE;
			index++;

			if (startDistance > endDistance)
			{
				swapEnds();
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	bool LineSegment::intersect(const LineSegment& s)
	{
		if (endDistance<s.startDistance+TOL || s.endDistance < this->startDistance + TOL)
			return false;
		return true;
	}

	std::string LineSegment::toString()
	{
		std::stringstream ss;
		ss << "LineSegment. Start: (" << startPos[0] << ", " << startPos[1] << ", " << startPos[2];
		ss << ") End: (" << endPos[0] << ", " << endPos[1] << ", " << endPos[2] << ")" << std::endl;
		ss << "   StartDistance: " << startDistance << ", EndDistance: " << endDistance << std::endl;
		return ss.str();
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

