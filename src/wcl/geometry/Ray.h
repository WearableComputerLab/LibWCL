/*-
 * Copyright (c) 2013 Tim Simon <tim.simon@radiumblue.net>
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


#ifndef WCL_RAY_H
#define WCL_RAY_H

#include <iostream>
#include <string>
#include <wcl/api.h>
#include <wcl/maths/Vector.h>
#include <wcl/geometry/Line.h>


namespace wcl
{
    /* 
     * Ray: a line with an origin extending in one direction.
     */
	class WCL_API Ray : virtual public wcl::Line
	{
		public:
			Ray(const wcl::Vector& startPos, const wcl::Vector& dir);

            wcl::Intersection intersect(const Ray& s);
            wcl::Intersection intersect(const Plane& p);

            bool isOnRay(const wcl::Vector&) const;

			/**
			 * Returns the closest point on this ray to the point.
			 */
			wcl::Vector closestPoint(const wcl::Vector& point) const;
			std::string toString();

            wcl::Vector getStart() const { return startPos; }

        private:
            wcl::Vector startPos;
	};
}

inline std::ostream& operator << (std::ostream& os, const wcl::Ray& l)
{
     return os << "<Ray: start " << l.getStart() << ", direction: " << l.getDirection() << ">";
}
#endif

