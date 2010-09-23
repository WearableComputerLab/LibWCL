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


#ifndef POLHEMUS_H
#define POLHEMUS_H

#include <string>
#include <wcl/api.h>
#include <wcl/tracking/PolhemusTrackedObject.h>
#include <wcl/tracking/Tracker.h>
#include <wcl/serial/Serial.h>

namespace wcl
{
	/**
	 * An interface for talking to the Polhemus Patriot tracking system.
	 *
	 * This class connects to the hardware via RS232 serial connection,
	 * and provides two tracked objects.
	 */
	class WCL_API Polhemus : public Tracker
	{
		public:
			enum TrackerType
			{
				PATRIOT,
				FASTRAK
			};

			/**
			 * Creates a new connection to the Patriot tracker
			 * over a serial connection.
			 *
			 * @param path The path to the serial connection (/etc/ttyUSB0 or something)
			 */
			Polhemus(std::string path, TrackerType t);

			/**
			 * DESTRUCTOR!
			 */
			virtual ~Polhemus();

			/**
			 * Fills the tracked objects witht he latest frame of data from the server.
			 */
			virtual void update();

			/**
			 * Returns the tracked object with the specified name.
			 *
			 * Note, the only valid names are sensor1 or sensor2
			 *
			 */
			virtual TrackedObject* getObject(std::string name);

			/**
			 * Return every possible object 
			 */
			virtual std::vector<TrackedObject *> getAllObjects();

			/**
			 * Set the hemisphere of operation for the system.
			 * The Patriot can only track in a hemisphere around the origin,
			 * this function sets the hemisphere. For example, 
			 *
			 * (0,0,1) represents the Z+ hemisphere.
			 * (0,0,0) puts the tracker in hemisphere tracking mode.
			 *
			 * @param hemisphere The vector representing the hemisphre of operation.
			 */
			void setHemisphere(const wcl::Vector& hemisphere);

			/**
			 * Sets the units of measurements used by the tracker.
			 *
			 * @param u The units to use.
			 */
			virtual void setUnits(Units u);

			/**
			 * Sets the sensor count.
			 *
			 * The Patriot autodetects the number of active sensors by querying the
			 * device. However, the FasTRAK does not support this feature, so you 
			 * have to specify the sensor count.
			 *
			 * This function can also be used to override the detected setting for 
			 * the Patriot, but this is not recommended.
			 *
			 * @param c The number of sensors attached to the tracker.
			 */
			void setSensorCount(int c);


			/**
			 * Sets the alignment reference frame of the device.
			 * 
			 * @param origin The position to be the new origin.
			 * @param xPos A position along the X axis from the origin.
			 * @param yPos A position along the Y axis from the origin.
			 */
			void setAlignmentFrame(const wcl::Vector& origin, const wcl::Vector& xPos, const wcl::Vector& yPos);

		private:
			/**
			 * The serial connection to the Patriot.
			 */
			wcl::Serial connection;

			/**
			 * The units we are currently using.
			 */
			Units units;

			/**
			 * Tells the patriot to send ASCII data.
			 */
			void setAsciiOutput();
			
			/**
			 * Puts the tracker into continuous mode.
			 * Currently not used.
			 */
			void setContinuous(bool c);

			/**
			 * Sets the data format to what we want.
			 * This class uses cartesian coordinates for position
			 * and a quaternion for orientation.
			 */
			void setDataFormat();

			/**
			 * Finds out how many sensors are connected.
			 */
			void getSensorCount();

			/**
			 * Clears the input stream.
			 */
			void clearInput();

			/**
			 * The number of sensors we have.
			 */
			int activeSensorCount;

			std::vector<PolhemusTrackedObject> sensors;

			/**
			 * Function to read any data that is left
			 * on the serial buffer.
			 */
			void readAll(std::string prefix);

			TrackerType type;

			bool continuous;
	};
}

#endif

