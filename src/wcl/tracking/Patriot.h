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


#ifndef PATRIOT_H
#define PATRIOT_H

#include <string>
#include <wcl/tracking/PatriotTrackedObject.h>
#include <wcl/serial/Serial.h>

namespace wcl
{
	class Patriot
	{
		public:
			enum Sensor
			{
				SENSOR1,
				SENSOR2
			};

			enum Units
			{
				INCHES,
				CM,
				MM
			};

			/**
			 * Creates a new connection to the Patriot tracker
			 * over a serial connection.
			 *
			 * @param path The path to the serial connection (/etc/ttyUSB0 or something)
			 */
			Patriot(std::string path);

			/**
			 * DESTRUCTOR!
			 */
			virtual ~Patriot();

			/**
			 * Fills the tracked objects witht he latest frame of data from the server.
			 */
			void update();

			/**
			 * Returns the tracked object with the specified name.
			 *
			 * Note, the only valid names are sensor1 or sensor2
			 *
			 */
			TrackedObject* getObject(std::string name);

			void setHemisphere(Sensor s, const wcl::Vector& hemisphere);
			void setUnits(Units u);

		private:
			wcl::Serial connection;
			Units units;
			void setAsciiOutput();
			void setContinuous();
			void setDataFormat();
			void getSensorCount();
			int activeSensorCount;
			PatriotTrackedObject sensor1;
			PatriotTrackedObject sensor2;
	};
}

#endif

