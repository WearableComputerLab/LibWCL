/*
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
 * Copyright (c) 2010 Markus Broecker <mbrckr@googlemail.com>
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
#include <iostream>

namespace wcl
{
	/**
	 * A Quaternion!
	 */
	struct Quaternion
	{
			T 	w, x, y, z;
		
			/// \name Construction
			/// \{

			/**
			 * Default constructor, creates the identity quaternion, w=1, xyz=0
			 * Probably don't want to use this one.
			 */
			Quaternion();

			Quaternion(T _w, T _x, T _y, T _z);

			/**
			 * Takes an axis of rotation and an angle (rads)
			 * and constructs a quaternion.
			 *
			 * @param axis The vector representing the axis of rotation.
			 * @param the amount of rotation, in radians.
			 */
			Quaternion(const Vector& axis, T angle);

			Quaternion(const Vector& v1, const Vector& v2);

			/// Creates a quaternion from a rotation matrix
			Quaternion(const SMatrix& m);
			
			/// Create a 'pure' quaternion from a vector. Used for rotations
			Quaternion(const Vector& v);
			
			
			/// \}
			
			/// \name Transformations
			/// \{
			
			/// Returns a 4x4 rotation matrix
			SMatrix getRotation() const;

			/// Rotates a given vector
			Vector rotate(const Vector& v) const;
			
			/// Rotates a quaternion
			Quaternion rotate(Quaternion q) const;


			/** Rotation quaternions are supposed to be of unit length. This
				method normalised the quaternion.
			*/
			void normalise();

			/// \}

			Quaternion operator * (const Quaternion& rhs) const;

			inline Quaternion getConjugate() const
			{
				return wcl::Quaternion(w, -x, -y, -z);
			}
			
			/**
			 * Sets the values of the quaternion.
			 */
			inline void set(T w_, T x_, T y_, T z_)
			{
				w = w_;
				x = x_;
				y = y_;
				z = z_;
			}
			
			
			/**
			 * Returns a string representation of the quaternion.
			 */
			std::string toString();


	};
}


inline std::ostream& operator << (std::ostream& os, const wcl::Quaternion& q)
{
	return os << "<Quaternion w" << q.w << " x" << q.x << " y" << q.y << " z" << q.z << ">"; 
}

#endif

