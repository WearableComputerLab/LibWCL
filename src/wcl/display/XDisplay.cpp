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

#include <assert.h>
#include <X11/extensions/dpms.h>

#define SERVER_DEFAULT (-1)
#define DEFAULT_TIMEOUT (-600)

namespace wcl {

XDisplay::XDisplay(const char *displaynameandport):
    connectionOwner(true)
{
    this->connection = XOpenDisplay( displaynameandport );
    if ( this->connection == NULL )
	throw new XException(this);
}

XDisplay::XDisplay(::Display *x11):
    connection(x11), connectionOwner(false)
{
    assert(x11 != NULL && "Invalid X11 Display Connection");
}

XDisplay::~XDisplay()
{
    if( this->connectionOwner)
	XCloseDisplay( this->connection );
}

void XDisplay::enableScreenSaver()
{
    int timeout, interval, blanking, exposure;
    XGetScreenSaver( this->connection, &timeout, &interval, &blanking, &exposure);
    XSetScreenSaver( this->connection, DEFAULT_TIMEOUT, SERVER_DEFAULT,
		     blanking, exposure );
}


void XDisplay::disableScreenSaver()
{
    int timeout, interval, blanking, exposure;
    XGetScreenSaver( this->connection, &timeout, &interval, &blanking, &exposure);
    XSetScreenSaver( this->connection, 0 /* Timeout */, 0 /* Interval */,
		     blanking, exposure);
}

void XDisplay::enableDPMS()
{
    DPMSEnable(this->connection);
}

void XDisplay::disableDPMS()
{
    DPMSDisable(this->connection);
}



void XDisplay::enableAll()
{
    this->enableDPMS();
    this->enableScreenSaver();
}

void XDisplay::disableAll()
{
    this->disableDPMS();
    this->disableScreenSaver();
}


::Display *XDisplay::getXConnection() const
{
    return this->connection;
}

unsigned XDisplay::getNumberOfScreens() const
{
    return ScreenCount(this->connection);
}

};
