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


namespace wcl
{
	LineSegment::LineSegment(const wcl::Vector& start, const wcl::Vector& end) : 
		startPos(start), endPos(end)
	{
	}


	bool LineSegment::intersect(const LineSegment& s)
	{
		//TODO Implement LineSegment's intersect
		return true;
	}

	std::string LineSegment::toString()
	{
		std::stringstream ss;
		ss << "LineSegment. Start: (" << startPos[0] << ", " << startPos[1] << ", " << startPos[2];
		ss << ") End: (" << endPos[0] << ", " << endPos[1] << ", " << endPos[2] << ")" << std::endl;
		return ss.str();
	}

	// Taken from Mathematics for Games and Interactive Applications
	wcl::Vector LineSegment::closestPoint(const wcl::Vector& point) const
	{
		Vector w = point - startPos;
		wcl::Vector direction = (endPos - startPos);
		double proj = w.dot(direction);
		if (proj <=0)
		{
			return startPos;
		}
		else
		{
			double vsq = direction.dot(direction);
			if (proj >= vsq)
				return startPos + direction;
			else
				return startPos + (proj/vsq)*direction;
		}
	}
}

