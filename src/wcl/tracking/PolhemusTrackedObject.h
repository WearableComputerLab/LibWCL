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

#ifndef POLHEMUS_TRACKED_OBJECT_H
#define POLHEMUS_TRACKED_OBJECT_H

#include <string>
#include <sstream>

#include <wcl/maths/Matrix.h>
#include <wcl/maths/SMatrix.h>
#include <wcl/maths/Vector.h>
#include <wcl/maths/Quaternion.h>
#include <wcl/tracking/TrackedObject.h>

namespace wcl 
{
	/**
	 * Represents an object that can be tracked by the Vicon system.
	 * 
	 */
	class PolhemusTrackedObject : public TrackedObject
	{
		public:
			PolhemusTrackedObject(const std::string& name);

			/**
			 * Destructor.
			 */
			virtual ~PolhemusTrackedObject();

			/**
			 * Returns a string representation of the object.
			 */
			virtual std::string toString() const;

			virtual SMatrix getTransform() const;

			virtual Vector getTranslation() const;
			
			virtual Quaternion getOrientation() const;

			virtual bool isVisible() const;

			void update(T x, T y, T z, T rw, T rx, T ry, T rz);
		private:
			wcl::Vector position;
			wcl::Quaternion orientation;
	};

};

#endif /*TRACKEDOBJECT_H_*/

