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
 *    documentation and/or other materials provided with the distribution.  * * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
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


#include <math.h>
#include <cstdlib>
#include <config.h>
#include <sstream>
#include <wcl/geometry/Line.h>
#include <wcl/Exception.h>

namespace wcl
{
	Line::Line(const wcl::Vector& position, const wcl::Vector& direction)
		: pos(position), dir(direction)
	{
		//just in case it's not a unit vector
		this->dir = this->dir.unit();
	}

	Line::Line(const Line& l)
	{
		this->pos = l.pos;
		this->dir = l.dir;
	}

	wcl::Vector Line::getPosition() const
	{
		return pos;
	}

	wcl::Vector Line::getDirection() const
	{
		return dir;
	}

	std::string Line::toString()
	{
		std::stringstream ss;
		ss << "Line. Position: (" << pos[0] << ", " << pos[1] << ", " << pos[2] << ") ";
		ss << "Direction: (" << dir[0] << ", " << dir[1] << ", " << dir[2] << ")" << std::endl;

		return ss.str();
	}

	double Line::distanceFromPoint(const wcl::Vector& p) const
	{
		return (this->pos - p).normal();
	}

	wcl::Vector Line::intersect(const wcl::Line& l) const
	{
		double t;
		if (fabs(dir[1] * l.dir[0] - dir[0] * l.dir[1]) > TOL)
		{
			t = (-pos[1]*l.dir[0] +
			      l.pos[1]*l.dir[0] +
				  l.dir[1]*pos[0] -
				  l.dir[1]*l.pos[0]) /
				(dir[1]*l.dir[0] - dir[0]*l.dir[1]);
		}
		else if (fabs(-dir[0] * l.dir[2] + dir[2]*l.dir[0]) > TOL)
		{
			t = -(-l.dir[2]*pos[0] +
				  l.dir[2]*l.pos[0] + 
				  l.dir[0]*pos[2] -
				  l.dir[0]*l.pos[2]) /
				(-dir[0]*l.dir[2]+dir[2]*l.dir[0]);
		}
		else if (fabs(-dir[2]*l.dir[1] + dir[1]*l.dir[2]) > TOL)
		{
			t = (pos[2]*l.dir[1] -
				 l.pos[2]*l.dir[1] -
				 l.dir[2]*pos[1] +
				 l.dir[2]*l.pos[1]) /
				(-dir[2]*l.dir[1] + dir[1]*l.dir[2]);
		}
		else
		{
			throw Exception("No Intersection!");
		}

		double x = pos[0] + dir[0]*t;
		double y = pos[1] + dir[1]*t;
		double z = pos[2] + dir[2]*t;

		return wcl::Vector(x,y,z);
	}

	wcl::Vector Line::intersect(const wcl::Plane& p) const
	{
        wcl::Vector normal = p.getNormal();
        float nDotPos = normal.dot(pos);
        float nDotDir = normal.dot(dir);

        // Denominator: nDotDir. 
        // Numerator: (p.getD() - nDotPos)

        // If the denominator is 0, we're either on the plane or 
        //  parallel to it. 
      
        float denominator = nDotDir;
        float numerator = (p.getD() - nDotPos);

        if ( fabs(denominator) < TOL ) 
            if ( fabs(numerator) < TOL ) // On the plane. 
                return pos;
            else // Parallel to plane. 
                throw Exception("No intersection.");

        // So now we're sure we have a single intersection point. 
        // Find the distance from the line position, and return it.
        return pos + ((numerator / denominator) * dir);
	}

	void Line::perturbDirection()
	{
		dir[0] += 1e-5*(((double)rand())/ RAND_MAX);
		dir[1] += 1e-5*(((double)rand())/ RAND_MAX);
		dir[2] += 1e-5*(((double)rand())/ RAND_MAX);
	}

    void Line::setDirection(const wcl::Vector& direction) {
        dir = direction;
    }
}

