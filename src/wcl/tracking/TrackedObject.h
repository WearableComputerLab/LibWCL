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

#ifndef WCL_TRACKING_TRACKEDOBJECT_H
#define WCL_TRACKING_TRACKEDOBJECT_H

#include <string>

#include <wcl/api.h>
#include <wcl/maths/Matrix.h>
#include <wcl/maths/Quaternion.h>
#include <wcl/maths/SMatrix.h>
#include <wcl/maths/Vector.h>

namespace wcl
{
	/**
	 * The type of object that we are tracking.
	 * Markers simply have position.
	 * Six DOF objects have position and rotation.
	 */
	enum ObjectType
	{
		/**
		 * A 3DOF Marker
		 */
		POSITION,

		/**
		 * Orientation marker only
		 */
		ORIENTATION,

		/**
		 * A 6DOF object
		 */
		SIX_DOF
	};

	/**
	 * Represents an object that can be tracked by the Vicon system.
	 *
	 */
	class WCL_API TrackedObject
	{
		public:
			/**
			 * Destructor.
			 */
			virtual ~TrackedObject(){}

			/**
			 * Returns a string representation of the object.
			 */
			virtual std::string toString() const = 0;

			/**
			 * Returns the name of the object.
			 */
			virtual std::string getName() const { return name;}

			/**
			 * Returns this object's type.
			 */
			virtual ObjectType getType() const { return type;}

			virtual SMatrix getTransform() const = 0;

			virtual Vector getTranslation() const = 0;

			virtual Quaternion getOrientation() const = 0;

			virtual bool isVisible() const = 0;

			virtual float getConfidence() const {return confidence;}

		protected:
			TrackedObject():
			    confidence(0.0){};

			/**
			 * The name of this object.
			 */
			std::string name;

			/**
			 * The type of object.
			 */
			ObjectType type;

			float confidence;
	};

};

#endif /*WCL_TRACKING_TRACKEDOBJECT_H_*/

