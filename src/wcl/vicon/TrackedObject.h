/*
 * Copyright (c) 2008
 * Michael Marner <marnermr@cs.unisa.edu.au>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef TRACKEDOBJECT_H_
#define TRACKEDOBJECT_H_

#include <string>

#include <wcl/network/TCPSocket.h>

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
		MARKER,

		/**
		 * A 6DOF object
		 */
		SIX_DOF
	};

	/**
	 * Represents an object that can be tracked by the Vicon system.
	 * 
	 */
	class TrackedObject
	{
		public:

			/**
			 * Constructor.
			 * @param name The name of the channel.
			 * @param type The type of tracked object.
			 */
			TrackedObject(std::string name, ObjectType type);

			/**
			 * Destructor.
			 */
			virtual ~TrackedObject();

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
			std::string toString();

			/**
			 * Returns the name of the object.
			 */
			std::string getName();

			/**
			 * Returns this object's type.
			 */
			ObjectType getType();


		protected:

			/**
			 * The name of this object.
			 */
			std::string name;

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
			bool occluded;
		
		private:
			/**
			 * The type of object.
			 */
			ObjectType type;

	};

};

#endif /*TRACKEDOBJECT_H_*/
