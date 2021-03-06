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


#ifndef PLANE_H
#define PLANE_H

#include <iostream>
#include <sstream>

#include <wcl/api.h>
#include <wcl/maths/Vector.h>
#include <wcl/geometry/Line.h>

namespace wcl
{
	struct WCL_API HNF
	{
		wcl::Vector n;
		double p;
	};

	class Line;

	/**
	 * A 3 dimensional, infinite plane.
	 */
	class WCL_API Plane
	{
		public:
			/**
			 * Creates a plane object such that v1, v2, and v3 lie on the plane.
			 */
			Plane(const wcl::Vector& v1, const wcl::Vector& v2, const wcl::Vector& v3);
            Plane(const wcl::Vector& point, const wcl::Vector& normal);

			/**
			 * Default constructor. Don't use this.
			 */
			Plane();

			/**
			 * Destructor.
			 */
			~Plane() {}

			/**
			 * Returns the minimum distance from p to the plane.
			 * Note that this is signed depending on whether p is in
			 * front or behind p.
			 */
			double distanceFrom(const wcl::Vector& p) const;

			/**
			 * Intersects this plane with another.
			 * Returns a PlaneIntersection, or NULL if the
			 * planes are parallel to each other.
			 */
			Line intersect(const Plane& p) const;


			wcl::Vector getPosition() const;
			wcl::Vector getNormal() const;
			double getD() const;

			/**
			 * Returns the Hessian Normal Form of this plane.
			 */
			HNF toHNF() const;

			std::string toString();

		private:
			/**
			 * The surface normal of the plane (A,B,C).
			 */
			wcl::Vector normal;

			/**
			 * The D value from the plane equation.
			 */
			double d;

			/**
			 * A point that lies on the plane.
			 */
			wcl::Vector point;
	};
} // namespace:wcl

inline std::ostream& operator << (std::ostream& os, const wcl::Plane& p)
{
    return os << "Point on plane: " << p.getPosition() << ", normal: " << p.getNormal();
}

#endif

