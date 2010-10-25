/*-
 * Copyright (c) 2009 Benjamin Close <Benjamin.Close@clearchain.com>
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
#include "XDisplay.h"

#include <X11/extensions/dpms.h>

#define SERVER_DEFAULT (-1)
#define DEFAULT_TIMEOUT (-600)

using namespace wcl;

/**
 * Open a connection to the XServer.
 *
 * @param displaynameandport A string with the display name and port ie: localhost:0
 * @throws XException on failed connection
 */
XDisplay::XDisplay(const char *displaynameandport):
    connectionOwner(true)
{
    this->connection = XOpenDisplay( displaynameandport );
    if ( this->connection == NULL )
	throw new XException(this);
}

/**
 * Turn a already open connection into an XDisplay object.
 * Note: Destroying the XDisplay object will not close the connection
 *       to the server in this case.
 *
 * @param x11 The already open connection
 */
XDisplay::XDisplay(::Display *x11):
    connection(x11), connectionOwner(false)
{}

/**
 * Destroy the XDisplay object. If the XConnection was created
 * by this object it will be closed before the object is destroyed
 */
XDisplay::~XDisplay()
{
    if( this->connectionOwner)
	XCloseDisplay( this->connection );
}

/**
 * Enable the screensaver under X windows
 */
void XDisplay::enableScreenSaver()
{
    int timeout, interval, blanking, exposure;
    XGetScreenSaver( this->connection, &timeout, &interval, &blanking, &exposure);
    XSetScreenSaver( this->connection, DEFAULT_TIMEOUT, SERVER_DEFAULT, 
		     blanking, exposure );
}


/**
 * Disable the Screen saver under X Windows
 * This is done by setting the timeout & interval to 0
 */
void XDisplay::disableScreenSaver()
{
    int timeout, interval, blanking, exposure;
    XGetScreenSaver( this->connection, &timeout, &interval, &blanking, &exposure);
    XSetScreenSaver( this->connection, 0 /* Timeout */, 0 /* Interval */,
		     blanking, exposure);
}

/**
 * Enable DPMS under X Windows
 */
void XDisplay::enableDPMS()
{
    DPMSEnable(this->connection);
}

/**
 * Enable DPMS under X Windows
 */
void XDisplay::disableDPMS()
{
    DPMSDisable(this->connection);
}


/**
 * Enable both the screen saver and DPMS
 */

void XDisplay::enableAll()
{
    this->enableDPMS();
    this->enableScreenSaver();
}

/**
 * Disable both the screen saver and DPMS
 */
void XDisplay::disableAll()
{
    this->disableDPMS();
    this->disableScreenSaver();
}


/**
 * Obtain the connection to the X server
 */
::Display *XDisplay::getXConnection() const
{
    return this->connection;
}
