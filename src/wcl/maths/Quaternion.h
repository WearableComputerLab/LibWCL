/*
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

#ifndef QUATERNION_H
#define QUATERNION_H

#include <wcl/maths/SMatrix.h>
#include <wcl/maths/Vector.h>
#include <string>
#include <sstream>

namespace wcl
{
	/**
	 * A Quaternion!
	 */
	class Quaternion
	{
		public:
			/**
			 * Default constructor, sets xyzw to 0.
			 * Probably don't want to use this one.
			 */
			Quaternion() {}

			/**
			 * Constructor.
			 * Sets xyzw to the parameters passed in.
			 *
			 * @param _w The w component.
			 * @param _x The x component.
			 * @param _y The y component.
			 * @param _z The z component.
			 */
			Quaternion(T _w, T _x, T _y, T _z);

			/**
			 * Takes an axis of rotation and an angle (rads)
			 * and constructs a quaternion.
			 *
			 * @param axis The vector representing the axis of rotation.
			 * @param the amount of rotation, in radians.
			 */
			Quaternion(const wcl::Vector& axis, T angle);

			Quaternion(const wcl::Vector& v1, const wcl::Vector& v2);

			wcl::SMatrix getRotation() const;

			/**
			 * Returns a string representation of the quaternion.
			 */
			std::string toString();
			
			/**
			 * Sets the values of the quaternion.
			 */
			void set(T w, T x, T y, T z);

			~Quaternion(){}

		private:
			T x;
			T y;
			T z;
			T w;

	};
}
#endif

