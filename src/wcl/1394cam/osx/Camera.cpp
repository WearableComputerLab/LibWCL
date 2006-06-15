/************************************************************************/
/*!
    @author Original Code Sample from Erik Staats, NWG.
            FWCamAkiz was written by Adrian Daerr

    Copyright (c) 2002 Adrian Daer & Université Paris VII "Denis Diderot" (France).									 
    Copyright (c) 2004 Joel Falcou & Université Clermont-Ferrand II "Blaise Pascal" (France).									 
    All rights reserved.																						 
    																																 
    This file is part of the C+FOX Library.  This
    library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation; either version 2.1,
    or (at your option) any later version.
    																						 
    This library is distributed in the hope that it will be useful,	 
    but WITHOUT ANY WARRANTY; without even the implied warranty of	 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		 
    GNU Lesser General Public License for more details.							 
    																																 
    You should have received a copy of the GNU Lesser General				 
    Public License along with this library; see the file COPYING.		 
    If not, send mail to the developers of C+FOX										 
    																																 
    As a special exception, you may use this file as part of a free	 
    software library without restriction. Specifically, if other		 
    files instantiate templates or use macros or inline functions		 
    from this file, or you compile this file and link it with other	 
    files to produce an executable, this file does not by itself		 
    cause the resulting executable to be covered by the GNU Lesser	 
    General Public License.  This exception does not however				 
    invalidate any other reasons why the executable file might be		 
    covered by the GNU Lesser General Public License.
*/
/************************************************************************/

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/ppc/kern_return.h>
#include <mach/mach_error.h>
#include <unistd.h>
#include <sys/time.h>

#include "formats.h"
#include "constants.h"
#include "Camera.h"
#include "Control.h"

using namespace cfox;

Camera& Camera::operator>>( unsigned char*& dst )
{
    dst = getFrame(MODE_FRAME,0,0);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
//
//			Default constructor
//
////////////////////////////////////////////////////////////////////////////////////
Camera::Camera()
{
    curFrame = NULL;
    curBuffer = 0;

    camControl = new Control;
}

Camera::Camera( const Settings& opt ) 
{
    curFrame = NULL;
    curBuffer = 0;

    camControl = new Control;
    *this << opt;
}

Camera::Camera( const Mode& m, const FPS& f, size_t channel, const Spec& sp)
{
CFMutableDictionaryRef  matchingDictionary = IOServiceMatching( "IOFireWireDevice" );

    curFrame = NULL;
    curBuffer = 0;

    camControl = new Control;
    *this << Settings(m,f,channel,sp);
}
////////////////////////////////////////////////////////////////////////////////////
//
//			Default destructor
//
//	Be careful to release all the things which need to be released...
//
////////////////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{
    if(curFrame) delete curFrame;
        
    if(camControl)
    {
        stop();
        delete camControl;
        camControl = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////
//
//			Start Method
//
////////////////////////////////////////////////////////////////////////////////////
IOReturn Camera::start()
{
	message( "Initializing camera" );
	int  result = noErr;

	// check that camControl is valid
	if( camControl )
	{
		message( "attempting to set video format" );

		message( "fps = %d", options.getFPS() );

		// basically copy the values to the control object
		camControl->setVideoFormat( options.getMode(), options.getFormat(), options.getFPS() );

		message( "attempting to initialize camera control" );
		result = camControl->init( options.isBroadcaster(),options.isSpecific(),options.getChannel() );
		if( result != noErr )
		{
			return result;
		}

		message( "width = %d", Width[ options.getMode() ][ options.getFormat() ] );
		message( "height = %d", Height[ options.getMode() ][ options.getFormat() ] );
		message( "channels = %d", VideoModeSize[ options.getMode() ][ options.getFormat() ] );
		message( "size of UInt8 = %ld", sizeof( UInt8 ) );

		curFrame = new UInt8[ Width[options.getMode()][options.getFormat()]*
			Height[options.getMode()][options.getFormat()]*
			VideoModeSize[options.getMode()][options.getFormat()] ];
	}
	else
	{
		gen_fatal( "camControl has not been initialize or has been destroyed" );
	}

	camControl->toggleCapture();

	return result;
}

////////////////////////////////////////////////////////////////////////////////////
//
//			Stop Method
//
////////////////////////////////////////////////////////////////////////////////////
IOReturn Camera::stop()
{
	return kIOReturnSuccess;
}

Camera::Frame Camera::getFrame( int mode, int frame_no, long time )
{
	camControl->getFrame( mode, frame_no, time, curFrame );
	return curFrame;
}

bool Camera::copyFrame( Camera::Frame fr, int mode, int frame_no, long time)
{
	if(fr) 
	{ 
		camControl->getFrame( mode, frame_no, time, fr );  
		return true;
	}
	else
	{
		fr = NULL;
		return false;
	}
}

size_t Camera::getSize()   const   { return camControl->getFrameSize(); }
size_t Camera::getWidth()  const   { return Width [camControl->getVideoFormat()][camControl->getVideoMode()]; }
size_t Camera::getHeight() const   { return Height[camControl->getVideoFormat()][camControl->getVideoMode()]; }
