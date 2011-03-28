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


#include <bluetooth/bluetooth.h>
#include <iostream>
#include <sstream>

#include "Wiimote.h"
#include "WiimoteException.h"

// Work around a warning caused by
// how BDADDR_ANY is defined in bluetooth.h/gcc being lax
//
// gcc requires the scope of any temporaries to be
// resolvable to an lval in order to not treat it as
// a temporary in expression in evaluation. By default
// BDADDR_ANY is defined in bluetooth.h as:
// #define BDADDR_ANY   (&(bdaddr_t) {{0, 0, 0, 0, 0, 0}})
// which whilst c98 defines as lval ("The
// operand [of the unary & operator] shall be an lvalue or a
// qualified-id ) g++ seems to be a little lax allowing it to be a
// rval. Hence a compiler warning is incorrectly generated. To get around this
// we instead uninline the construct and define a file bound version of the variable
// which will never be temporary. Bad g++.. this is a feature not a bug :?
#ifdef BDADDR_ANY
#undef BDADDR_ANY
static bdaddr_t any = {{0, 0, 0, 0, 0, 0}};
#define BDADDR_ANY &any
#endif


using namespace std;

namespace wcl
{

	Wiimote::Wiimote() throw(WiimoteException)
		: mMessageQueue(NULL), mAccelerometer(3), mNunchukStick(2), mNunchukAccelerometer(3), mButtonState(0)
	{
		//connect to wiimote
		bacpy(&mMacAddress, BDADDR_ANY);
		mWiimote = cwiid_open(&mMacAddress, CWIID_FLAG_CONTINUOUS | CWIID_FLAG_MESG_IFC | CWIID_FLAG_REPEAT_BTN | CWIID_FLAG_NONBLOCK);

		if (mWiimote == NULL)
			throw WiimoteException("Could not find a Wiimote to connect to.", WiimoteException::CONNECTION_TIMEOUT);

		cwiid_command(mWiimote, CWIID_CMD_RPT_MODE, CWIID_RPT_ACC|CWIID_RPT_STATUS|CWIID_RPT_BTN|CWIID_RPT_NUNCHUK|CWIID_RPT_IR );
		mMessageQueue = new cwiid_mesg[5];

	}


	Wiimote::Wiimote(std::string mac) throw(WiimoteException)
		: mMessageQueue(NULL), mAccelerometer(3), mNunchukStick(2), mNunchukAccelerometer(3), mButtonState(0)
	{
		if (0 != str2ba(mac.c_str(), &mMacAddress))
		{
			stringstream ss;
			ss << "The MAC address: ";
			ss << mac;
			ss << " is invalid, can not connect to wiimote.";
			throw WiimoteException(ss.str().c_str(), WiimoteException::BAD_MAC);
		}

		mWiimote = cwiid_open(&mMacAddress, CWIID_FLAG_CONTINUOUS | CWIID_FLAG_MESG_IFC | CWIID_FLAG_REPEAT_BTN | CWIID_FLAG_NONBLOCK);

		if (mWiimote == NULL)
		{
			stringstream ss;
			ss << "Connection timed out when trying to connect to wiimote: ";
			ss << mac;
			throw WiimoteException(ss.str().c_str(), WiimoteException::CONNECTION_TIMEOUT);
		}

		cwiid_command(mWiimote, CWIID_CMD_RPT_MODE, CWIID_RPT_ACC|CWIID_RPT_STATUS|CWIID_RPT_BTN|CWIID_RPT_NUNCHUK|CWIID_RPT_IR );
		mMessageQueue = new cwiid_mesg[5];
	}


	Wiimote::~Wiimote()
	{
		cwiid_close(mWiimote);

		// delete the message queue if it has been initialised.
		if (mMessageQueue != NULL)
			delete [] mMessageQueue;
	}


	void Wiimote::update()
	{
		int messageCount;
		timespec spec;

		int returnVal = cwiid_get_mesg(mWiimote, &messageCount, &mMessageQueue, &spec);

		if (0 == returnVal)
		{
			for (int i=0;i<messageCount; ++i)
			{
				switch (mMessageQueue[i].type)
				{
					case CWIID_MESG_BTN:
						{
							// first we zero out the current button state for everything
							// except the nunchuk buttons...
							uint16_t bitmask = buttonToBitmask(BUTTON_C) | buttonToBitmask(BUTTON_Z);
							mButtonState = mButtonState & bitmask;

							//now we OR that with the new state, so we get button and nunchuck
							mButtonState = mMessageQueue[i].btn_mesg.buttons | mButtonState;
						}
						break;

					case CWIID_MESG_NUNCHUK:
						mNunchukStick[0] = (double) mMessageQueue[i].nunchuk_mesg.stick[0];
						mNunchukStick[1] = (double) mMessageQueue[i].nunchuk_mesg.stick[1];

						mNunchukAccelerometer[0] = (double) (mMessageQueue)[i].nunchuk_mesg.acc[0];
						mNunchukAccelerometer[1] = (double) (mMessageQueue)[i].nunchuk_mesg.acc[1];
						mNunchukAccelerometer[2] = (double) (mMessageQueue)[i].nunchuk_mesg.acc[2];

						//button c
						if ((mMessageQueue[i].nunchuk_mesg.buttons & CWIID_NUNCHUK_BTN_C) == 0)
						{
							// it is zero, so XOR
							mButtonState = mButtonState & (0xffff ^ buttonToBitmask(BUTTON_C));
						}
						else
						{
							// it is on, so OR
							mButtonState = mButtonState | buttonToBitmask(BUTTON_C);
						}

						//button z
						if ((mMessageQueue[i].nunchuk_mesg.buttons & CWIID_NUNCHUK_BTN_Z) == 0)
						{
							// it is zero, so XOR
							mButtonState = mButtonState & (0xffff ^ buttonToBitmask(BUTTON_Z));
						}
						else
						{
							// it is on, so OR
							mButtonState = mButtonState | buttonToBitmask(BUTTON_Z);
						}

						break;

					case CWIID_MESG_ACC:
						mAccelerometer = wcl::Vector((double) (mMessageQueue)[i].acc_mesg.acc[0],
								(double) (mMessageQueue)[i].acc_mesg.acc[1],
								(double) (mMessageQueue)[i].acc_mesg.acc[2]);
						break;

					case CWIID_MESG_ERROR:
						if (mMessageQueue[i].error_mesg.error == CWIID_ERROR_DISCONNECT)
						{
							throw WiimoteException("Connection to wiimote has failed.", WiimoteException::DISCONNECT);
						}
						break;

					default:
						break;
				}
			}
		}
	}

	void Wiimote::reconnect() throw(WiimoteException)
	{
		mWiimote = cwiid_open(&mMacAddress, CWIID_FLAG_CONTINUOUS | CWIID_FLAG_MESG_IFC | CWIID_FLAG_REPEAT_BTN | CWIID_FLAG_NONBLOCK);

		if (mWiimote == NULL)
		{
			stringstream ss;
			ss << "Connection timed out when trying to connect to wiimote.";
			throw WiimoteException(ss.str().c_str(), WiimoteException::CONNECTION_TIMEOUT);
		}

		cwiid_command(mWiimote, CWIID_CMD_RPT_MODE, CWIID_RPT_ACC|CWIID_RPT_STATUS|CWIID_RPT_BTN|CWIID_RPT_NUNCHUK|CWIID_RPT_IR );
	}

	void Wiimote::disconnect()
	{
		cwiid_close(mWiimote);
	}

	wcl::Vector Wiimote::getAccelerometer() const
	{
		return mAccelerometer;
	}

	wcl::Vector Wiimote::getNunchukAccelerometer() const
	{
		return mNunchukAccelerometer;
	}


	wcl::Vector Wiimote::getNunchukStick() const
	{
		return mNunchukStick;
	}

	bool Wiimote::getButton(Button b) const
	{
		return (0 != (buttonToBitmask(b) & mButtonState));
	}

	uint16_t Wiimote::getRawButtonState() const
	{
		return mButtonState;
	}

	
	std::string Wiimote::getMacAddress() const
	{
		char s[18];
		ba2str(&mMacAddress, s);
		return s;
	}

	int Wiimote::getSoftwareID() 
	{
		return cwiid_get_id(mWiimote);
	}



	uint16_t Wiimote::buttonToBitmask(Button b) const
	{
		uint16_t cwiid[] = {CWIID_BTN_2,
			CWIID_BTN_1,
			CWIID_BTN_B,
			CWIID_BTN_A,
			CWIID_BTN_MINUS,
			CWIID_BTN_HOME,
			CWIID_BTN_LEFT,
			CWIID_BTN_RIGHT,
			CWIID_BTN_DOWN,
			CWIID_BTN_UP,
			CWIID_BTN_PLUS,
			0x2000,
			0x4000};

		return cwiid[b];
	}

	void Wiimote::setLED(int led, bool on) throw(WiimoteException)
	{
		unsigned char leds[] = {CWIID_LED1_ON,
			CWIID_LED2_ON,
			CWIID_LED3_ON,
			CWIID_LED4_ON};

		if (on)
		{
			mLEDState = mLEDState | leds[led];
		}
		else if (!on && 0 != (mLEDState & leds[led]))
		{
			//only change the flag if necessary...
			mLEDState = mLEDState ^ leds[led];
		}

		if (0 != cwiid_command(mWiimote, CWIID_CMD_LED, mLEDState))
		{
			throw WiimoteException("Could not set LED state.", WiimoteException::COMMAND_FAILURE);
		}
	}

}

