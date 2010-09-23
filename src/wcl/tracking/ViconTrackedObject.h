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

#ifndef VICON_TRACKEDOBJECT_H
#define VICON_TRACKEDOBJECT_H

#include <string>

#include <wcl/api.h>
#include <wcl/network/TCPSocket.h>
#include <wcl/maths/Matrix.h>
#include <wcl/maths/SMatrix.h>
#include <wcl/maths/Vector.h>
#include <wcl/tracking/TrackedObject.h>
#include <wcl/tracking/Tracker.h>

namespace wcl 
{
	/**
	 * Represents an object that can be tracked by the Vicon system.
	 */
	class WCL_API ViconTrackedObject : public TrackedObject
	{
		public:
			/**
			 * Constructor.
			 * @param name The name of the channel.
			 * @param type The type of tracked object.
			 */
			ViconTrackedObject(std::string name, ObjectType type);

			/**
			 * Destructor.
			 */
			virtual ~ViconTrackedObject();

			/**
			 * Updates the data stored in this object.
			 * 
			 * @param array The array of data to read from
			 * @param offset The location in the array to start reading from. Note that
			 *               the object will increment this based on how many values
			 *               it reads. 6 for 6DOF objects, 4 for markers.
			 */
			void updateData(double* array, int & offset);

			/**
			 * Returns a string representation of the object.
			 */
			virtual std::string toString() const;

			virtual SMatrix getTransform() const;

			virtual Vector getTranslation() const;
			
			virtual Quaternion getOrientation() const;
			SMatrix getRotationMatrix() const;

			void setUnits(Tracker::Units u);

			virtual bool isVisible() const;

		protected:
			/**
			 * Location of this object in x.
			 */
			double x;

			/**
			 * Location of this object in y.
			 */
			double y;

			/**
			 * Location of this object in z.
			 */
			double z;

			double rx;
			double ry;
			double rz;

			/**
			 * Whether the object is visible or not.
			 * This only applies to Markers, 6DOF objects do not
			 * receive this information from the server.
			 */
			bool visible;

			Tracker::Units units;
	};

};

#endif 

