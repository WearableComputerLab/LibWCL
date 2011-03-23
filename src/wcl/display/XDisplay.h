/*-
 * Copyright (c) 2005-2008 Benjamin Close <Benjamin.Close@clearchain.com>
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
#ifndef WCL_DISPLAY_XDISPLAY_H
#define WCL_DISPLAY__XDISPLAY_H

#include <string>
#include <X11/Xlib.h>
#include <wcl/api.h>
#include <wcl/Exception.h>

namespace wcl {

/**
 * This class provides simple routines for
 * Interating with an X11 Display
 */
class WCL_API XDisplay
{
    public:
	/**
	 * Open a connection to the XServer.
	 *
	 * @param displaynameandport A string with the display name and port ie: localhost:0
	 * @throws XException on failed connection
	 */
	XDisplay(const char *displaynameandport);


	/**
	 * Turn a already open connection into an XDisplay object.
	 * Note: Destroying the XDisplay object will not close the connection
	 *       to the server in this case.
	 *
	 * @param x11 The already open connection
	 */
	XDisplay(::Display *); //Pass in an X Display

	/**
	 * Destroy the XDisplay object. If the XConnection was created
	 * by this object it will be closed before the object is destroyed
	 */
	~XDisplay();

	/**
	 * Enable the screensaver under X windows
	 */
	void enableScreenSaver();

	/**
	 * Enable DPMS under X Windows
	 */
	void enableDPMS();

	/**
	 * Disable the Screen saver under X Windows
	 * This is done by setting the timeout & interval to 0
	 */
	void disableScreenSaver();

	/**
	 * Enable DPMS under X Windows
	 */
	void disableDPMS();

	/**
	 * Enable both the screen saver and DPMS
	 */
	void enableAll();

	/**
	 * Disable both the screen saver and DPMS
	 */
	void disableAll();

	/**
	 * Obtain the connection to the X server
	 */
	::Display *getXConnection() const;

	/**
	 * Obtain the number of screens available on this display
	 */
	unsigned getNumberOfScreens() const;

    private:
	::Display *connection; // X Windows Connection
	bool connectionOwner;  // Whether this object is the connection owner or not

};

class WCL_API XException: Exception
{
public:
    XException(const XDisplay *) throw (){};
    virtual ~XException() throw (){};

    int getCause() const { return 1;};
    const std::string getReason() const { return "Failed To Connect";};
};

}


#endif
