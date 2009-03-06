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

namespace wcl {

/**
 * This class provides simple routines for
 * Interating with an X11 Display
 */
class XDisplay
{
    public:
	XDisplay(const char *displaynameandport);
	XDisplay(::Display *); //Pass in an X Display
	~XDisplay();

	void enableScreenSaver();
	void enableDPMS();
	void disableScreenSaver();
	void disableDPMS();

	void enableAll();
	void disableAll();

	::Display *getXConnection() const;

    private:
	::Display *connection; // X Windows Connection
	bool connectionOwner;  // Whether this object is the connection owner or not

};

class XException
{
public:
    XException(const XDisplay *){};
    virtual ~XException(){};

    int getCause() const { return 1;};
    const std::string getReason() const {"Failed To Connect";};
};

}


#endif
