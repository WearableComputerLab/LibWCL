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

#include <wcl/maths/Vector.h>

namespace wcl
{
	/**
	 * Struct used to represent a Plane-Plane intersection.
	 */
	struct PlaneIntersection
	{
		/**
		 * A point on the line of intersection.
		 */
		wcl::Vector p;

		/**
		 * The direction of the line of intersection./
		 */
		wcl::Vector dir;
	};

	struct HNF
	{
		wcl::Vector n;
		double p;
	};

	/**
	 * A 3 dimensional, infinite plane.
	 */
	class Plane
	{
		public:
			/**
			 * Creates a plane object such that v1, v2, and v3 lie on the plane.
			 */
			Plane(const wcl::Vector& v1, const wcl::Vector& v2, const wcl::Vector& v3);

			/**
			 * Destructor.
			 */
			~Plane();

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
			PlaneIntersection intersect(const Plane& p) const;

			/**
			 * Returns the Hessian Normal Form of this plane.
			 */
			HNF toHNF() const;

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
}

#endif

