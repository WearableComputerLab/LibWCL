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

#include <limits>
#include <assert.h>

#include "BoundingBox.h"

namespace wcl
{

	BoundingBox::BoundingBox(const wcl::Vector& minimum, const wcl::Vector& maximum) :
		min(minimum), max(maximum)
	{
		assert(minimum.getRows() == 3);
		assert(maximum.getRows() == 3);
		// hello!
	}

	BoundingBox::BoundingBox()
	{
		double maxDouble = std::numeric_limits<double>::max();
		min = wcl::Vector(maxDouble,maxDouble,maxDouble);

		double minDouble = std::numeric_limits<double>::min();
		max = wcl::Vector(minDouble,minDouble,minDouble);
	}


	BoundingBox::BoundingBox(const std::vector<Vertex*>& verts)
	{
		double maxDouble = std::numeric_limits<double>::max();
		min = wcl::Vector(maxDouble,maxDouble,maxDouble);

		double minDouble = std::numeric_limits<double>::min();
		max = wcl::Vector(minDouble,minDouble,minDouble);

		std::vector<Vertex*>::const_iterator it;
		for (it = verts.begin(); it<verts.end(); ++it)
		{
			assert((*it)->position.getRows() == 3);
			this->addPoint((*it)->position);
		}
	}


	bool BoundingBox::overlaps(const wcl::BoundingBox& b) const
	{
		//this is going to be a big if statement
		if (this->max[0] <= b.min[0] ||
			this->max[1] <= b.min[1] ||
			this->max[2] <= b.min[2] ||
			b.max[0] <= this->min[0] ||
			b.max[1] <= this->min[1] ||
			b.max[2] <= this->min[2])
		{
			return false;
		}
		return true;
	}


	bool BoundingBox::intersect(const wcl::LineSegment& segment) const
	{
		//direction from start to end
		wcl::Vector dir = segment.endPos - segment.startPos;
		//length of segment
		double minT = dir.normal(); 
		double maxS = 0.0;
		dir = dir.unit();
		double s, t;
		
		//check X
		double recipX = 1.0/dir[0];
		if (recipX >= 0.0)
		{
			s = (min[0] - segment.startPos[0])*recipX;
			t = (max[0] - segment.startPos[0])*recipX;
		}
		else
		{
			s = (max[0] - segment.startPos[0])*recipX;
			t = (min[0] - segment.startPos[0])*recipX;
		}

		if (s>maxS)
			maxS = s;
		if (t<minT)
			minT = t;
		if (maxS > minT)
			return false;

		//check Y
		recipX = 1.0/dir[1];
		if (recipX >= 0.0)
		{
			s = (min[1] - segment.startPos[1])*recipX;
			t = (max[1] - segment.startPos[1])*recipX;
		}
		else
		{
			s = (max[1] - segment.startPos[1])*recipX;
			t = (min[1] - segment.startPos[1])*recipX;
		}

		if (s>maxS)
			maxS = s;
		if (t<minT)
			minT = t;
		if (maxS > minT)
			return false;

		//check Z
		recipX = 1.0/dir[2];
		if (recipX >= 0.0)
		{
			s = (min[2] - segment.startPos[2])*recipX;
			t = (max[2] - segment.startPos[2])*recipX;
		}
		else
		{
			s = (max[2] - segment.startPos[2])*recipX;
			t = (min[2] - segment.startPos[2])*recipX;
		}

		if (s>maxS)
			maxS = s;
		if (t<minT)
			minT = t;
		if (maxS > minT)
			return false;

		return true;
	}


	void BoundingBox::addPoint(const wcl::Vector& p)
	{
		assert(p.getRows() == 3);
		//test x
		if (p[0] < min[0])
			min[0] = p[0];

		if (p[0] > max[0])
			max[0] = p[0];

		//test y
		if (p[1] < min[1])
			min[1] = p[1];

		if (p[1] > max[1])
			max[1] = p[1];

		//test z
		if (p[2] < min[2])
			min[2] = p[2];

		if (p[2] > max[2])
			max[2] = p[2];
	}
}

