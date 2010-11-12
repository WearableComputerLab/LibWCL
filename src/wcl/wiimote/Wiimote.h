/*-
 * Copyright (c) 2010 Michael Marner <michael@20papercups.net>
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


#ifndef WIIMOTE_H
#define WIIMOTE_H

#include <string>
#include <cwiid.h>
#include <stdint.h>
#include <wcl/maths/Vector.h>

#include "WiimoteException.h"

namespace wcl
{
	/**
	 * A class that wraps a Wiimote connection using libcwiid.
	 */
	class WCL_API Wiimote
	{

		public:

			/**
			 * Enumeration for all of the buttons on the wiimote
			 * and nunchuk.
			 */
			enum Button
			{
				BUTTON_2 = 0,
				BUTTON_1,
				BUTTON_B,
				BUTTON_A,
				BUTTON_MINUS,
				BUTTON_HOME,
				BUTTON_LEFT,
				BUTTON_RIGHT,
				BUTTON_DOWN,
				BUTTON_UP,
				BUTTON_PLUS,
				BUTTON_C, //nunchuck
				BUTTON_Z //nunchuck
			};

			/**
			 * Opens a connection to a wiimote.
			 * This constructor connects to the first available wiimote it sees,
			 * as such, you do not need to know the mac address to create a connection.
			 */
			Wiimote() throw(WiimoteException);

			/**
			 * Opens a connection to a specific wiimote.  If you know the MAC
			 * address of the wiimote you wish to connect to, then use this
			 * function.
			 *
			 * @param mac The mac address to attemp to connect to.
			 */
			Wiimote(std::string mac) throw(WiimoteException);

			/**
			 * Destructor. Closes the connection if it is still open, 
			 * does necessary cleanup.
			 */
			~Wiimote();

			/**
			 * Queries the wiimote and updates the state as needed.
			 * Internally, this function makes non-blocking calls, so this
			 * method will return immediately.
			 */
			void update();

			/**
			 * Closes the connection to the wiimote.
			 */
			void disconnect();

			/**
			 * Reestablish a connection to the same wiimote.
			 */
			void reconnect() throw(WiimoteException);


			/**
			 * Turns an LED on or off.
			 *
			 * @param led The led number (0-3) to set.
			 * @param on True to turn the led on, false to turn off.
			 */
			void setLED(int led, bool on) throw(WiimoteException);

			/**********************************
			 *     WIIMOTE DATA ACCESSORS
			 *********************************/

			/**
			 * Gets the accelerometer data.
			 */
			wcl::Vector getAccelerometer() const;


			/**
			 * Gets the accelerometer data from the nunchuk,
			 * or a zero vector if it is not plugged in.
			 */
			wcl::Vector getNunchukAccelerometer() const;


			/**
			 * Gets the X/Y position of the nunchuk stick,
			 * or (0,0) if it is not plugged int.
			 */
			wcl::Vector getNunchukStick() const;

			/**
			 * Gets the state of a button.
			 * 
			 * @param b The button to query.
			 * @return True if the button is down, false otherwise.
			 */
			bool getButton(Button b) const;


			/**
			 * Returns the raw button state as a uint16.
			 * Each button is bitmasked.
			 */
			uint16_t getRawButtonState() const;

			/**
			 * Returns the string representation of this wiimotes
			 * MAC address.
			 */
			std::string getMacAddress() const;

			/**
			 * Returns the software ID assigned to this wiimote.
			 * This is unique amoung all wiimotes for a process,
			 * but each wiimote may receive a different ID each
			 * time the process starts.
			 */
			int getSoftwareID();


		private:
			/*
			 * Cwiid stuff
			 */
			bdaddr_t mMacAddress;
			cwiid_wiimote_t * mWiimote;
			cwiid_mesg* mMessageQueue;

			/*
			 * Wiimote data
			 */
			wcl::Vector mAccelerometer;
			wcl::Vector mNunchukStick;
			wcl::Vector mNunchukAccelerometer;

			/**
			 * The buttonstate from cwiid, with two changes:
			 *
			 * Cwiid uses a completely different struct for
			 * nunchuk buttons. This is kind of a pain for
			 * us, we want to use a single field.
			 *
			 * Luckily, there is still room for the two extra
			 * buttons. Therefore, we do the following:
			 *
			 *    BUTTON_C = 0x2000
			 *    BUTTON_Z = 0x4000
			 *
			 */
			uint16_t mButtonState;

			/**
			 * Battery status.
			 * 1.0 == full battery
			 * 0.0 == dead
			 */
			float mBattery;

			/**
			 * The current state of the LEDs.
			 */
			unsigned char mLEDState;

			/*
			 * Helper Functions...
			 */

			/**
			 * Converts a Button value into a bitmask
			 * that can be used against the mButtonState
			 * in order to obtain the state of a single button.
			 */
			uint16_t buttonToBitmask(Button b) const;

	};

}

#endif
