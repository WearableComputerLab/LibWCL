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


#ifndef VEC3_H
#define VEC3_H

#include <wcl/maths/Vector.h>

namespace wcl
{

	/**
	 * A wrapper around the wcl Vector for a 3 value vector.
	 * This class exists simply to make working with the Vector easier,
	 * since a lot of the time we are using 3 value vectors.
	 *
	 * @author Michael Marner
	 */
	class Vec3 : public Vector
	{

		public: 

			/**
			 * Creates a new Vec3, taking the values as parameters.
			 *
			 * @param x The x value.
			 * @param y The y value.
			 * @param z The z value.
			 */
			Vec3(T x, T y, T z);


			/**
			 * Returns the X value.
			 */
			T x();
			
			/**
			 * Returns the Y value.
			 */
			T y();

			/**
			 * Returns the Z value.
			 */
			T z();

			/**
			 * Sets the X value.
			 */
			void x(T x);
			
			/**
			 * Sets the Y value.
			 */
			void y(T y);

			/**
			 * Sets the Z value.
			 */
			void z(T z);


	};

};

#endif

