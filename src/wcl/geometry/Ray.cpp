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


#include <sstream>
#include <iostream>
#include <cstdlib>
#include <config.h>
#include <wcl/geometry/Ray.h>
#include <wcl/geometry/Intersection.h>
#include <float.h>

namespace wcl
{
	Ray::Ray(const wcl::Vector& start, const wcl::Vector& dir) : 
		Line(start, dir), startPos(start)
	{
	}


    wcl::Intersection Ray::intersect(const Ray& s)
	{
        wcl::Intersection ip = Line::intersect((wcl::Line)s);

        if ( ip.intersects == wcl::Intersection::IS_SAME ) {
            return ip;
        } else if ( ip.intersects == wcl::Intersection::NO ) {
            return ip;
        } else if ( ip.intersects == wcl::Intersection::YES ) {
            if ( isOnRay(ip.point) )
                return ip;
            else {
                ip.intersects == wcl::Intersection::NO;
                return ip;
            }
        }
	}

    wcl::Intersection Ray::intersect(const Plane& p) {
        wcl::Intersection ip = ((Line*) this)->intersect(p);
        if ( ip.intersects == wcl::Intersection::NO ) {
            return ip;
        } 
        else if ( ip.intersects == wcl::Intersection::YES ) {
            if ( isOnRay(ip.point) )
                return ip;
            else {
                ip.intersects == wcl::Intersection::NO;
                return ip;
            }
        }
    }


    bool Ray::isOnRay(const wcl::Vector& point) const {

        /*
         * We find the cross product of (point-start) X ( end - start). If it's zero, they're co-linear. 
         */
        wcl::Vector endPos = startPos + (dir * DBL_MAX) ;
        wcl::Vector pSubStart = point - startPos;
        wcl::Vector endSubStart = endPos - startPos;

        wcl::Vector result = pSubStart.crossProduct(endSubStart);
        if ( result.length() < TOL ) {
            /*
             * Now, we need to check if the point is between start and end. 
             */
            double product = pSubStart.dot(endSubStart);
            /*
             * If the product of the scalar is non-negative, the point is coincident or between start
             * and end. Otherwise it's outside the line segment. 
             */
            if ( product < 0 ) return false; 
                return true; 
        }
        return false;
    }

	std::string Ray::toString()
	{
		std::stringstream ss;
		ss << "Ray. Start: (" << startPos << ", direction: " << dir;
		ss << ") ";
		return ss.str();
	}

	// Taken from Mathematics for Games and Interactive Applications
	wcl::Vector Ray::closestPoint(const wcl::Vector& point) const
	{
        wcl::Vector endPos = startPos * DBL_MAX;
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

