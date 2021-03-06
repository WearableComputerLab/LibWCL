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


#ifndef WCL_TRACKING_TRACKER_H
#define WCL_TRACKING_TRACKER_H

#include <vector>

#include <wcl/api.h>
#include <wcl/tracking/TrackedObject.h>

namespace wcl
{

	/**
	 * Abstract Base Class that Trackers should extend.
	 * Provides (as best possible) a unified interface for all
	 * tracker types.
	 */
	class WCL_API Tracker
	{
		public:
			/**
			 * The units to receive values in.
			 */
			enum Units
			{
				/**
				 * Patriot returns positional data in inches.
				 */
				INCHES,

				/**
				 * Patriot returns positional data in cm.
				 */
				CM,

				/**
				 * Patriot returns positional data in mm.
				 * Note that the Patriot hardware doesn't actually support
				 * mm, this is handled internally by multiplying cm values
				 * by 10.
				 */
				MM
			};

			Tracker(){}
			virtual ~Tracker(){}

			/**
			 * Fills the tracked objects witht he latest frame of data from the server.
			 */
			virtual void update() = 0;


			/**
			 * Returns the tracked object with the specified name.
			 *
			 * Note, the only valid names are sensor1 or sensor2 for
			 * vicon & polhemus
			 *
			 */
			virtual TrackedObject* getObject(std::string name) = 0;

			/**
			 * Returns all the avaliable Tracked objects
			 */
			virtual std::vector<TrackedObject *> getAllObjects() = 0;


			/**
			 * Sets the units of measurements used by the tracker.
			 *
			 * @param u The units to use.
			 */
			virtual void setUnits(Units u) = 0;

	};

}

#endif

