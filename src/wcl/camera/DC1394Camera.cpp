/*-
 * Copyright (c) 2006 Aaron Stafford 2006 <aaron@staffordfarm.com.au>
 * Copyright (c) 2010 Benjamin Close <Benjamin.Close@clearchain.com>
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
#include <stdio.h>
#include <dc1394/dc1394.h>
#include <dc1394/utils.h>
#include <stdlib.h>
#include "DC1394Camera.h"
#include <unistd.h>
#include <string.h>
#include <iostream>

#warning DC1394Camera: Note This class needs some major cleanup -benjsc 20100205

using namespace std;

namespace wcl {

DC1394Camera::DC1394Camera(const uint64_t myguid):
    frame(NULL),
    d(NULL),
    running(false),
    guid(myguid)
{
	this->d = dc1394_new ();
	if( !this->d )
	    throw std::string("DC1394Camera:DC1394Camera: Failed to init DC1394 Library");
	printf("%lu\n", guid);
	this->camera = dc1394_camera_new( this->d, myguid );
	if( !this->camera )
	    throw std::string("DC1394Camera:DC1394Camera: Unknown Camera");

	// Just a safety thing, 1394 is a little touchy
	dc1394_reset_bus (this->camera);

	this->getCurrentConfig();
}

DC1394Camera::~DC1394Camera()
{
	this->shutdown();
	dc1394_camera_free( this->camera );
	dc1394_free(this->d);
}

void DC1394Camera::setFormat( const ImageFormat format, const unsigned width,
			const unsigned height )
{
    dc1394video_mode_t mode = DC1394_VIDEO_MODE_640x480_RGB8; // Default

#warning DC1349:setFormat This methods needs work.. benjsc - 20100205
    //XXX This needs to be redone
    switch( format ){
	case YUYV422:
	    if( width == 160 ) { mode = DC1394_VIDEO_MODE_160x120_YUV444; break; }
	    if( width == 320 ) { mode = DC1394_VIDEO_MODE_320x240_YUV422; break; }
	    if( width == 640 ){  mode = DC1394_VIDEO_MODE_640x480_YUV422; break; }
	    if( width == 800 ) { mode = DC1394_VIDEO_MODE_800x600_YUV422; break; }
	    if( width == 1024 ) { mode = DC1394_VIDEO_MODE_1024x768_YUV422; break; }
	    if( width == 1280 ) { mode = DC1394_VIDEO_MODE_1280x960_YUV422; break; }
	    if( width == 1600) { mode = DC1394_VIDEO_MODE_1600x1200_YUV422; break; }
	    goto notfound;
	case YUYV411:
	    if( width == 640 ) { mode = DC1394_VIDEO_MODE_640x480_YUV411; break; }
	    goto notfound;
	case RGB8:
	    if( width == 640 ) { mode = DC1394_VIDEO_MODE_640x480_RGB8; break; }
	    if( width == 800 ) { mode = DC1394_VIDEO_MODE_800x600_RGB8; break; }
	    if( width == 1024 ) { mode = DC1394_VIDEO_MODE_1024x768_RGB8; break; }
	    if( width == 1280 ) { mode = DC1394_VIDEO_MODE_1280x960_RGB8; break; }
	    if( width == 1600) { mode = DC1394_VIDEO_MODE_1600x1200_RGB8; break; }
	    goto notfound;
	case MONO8:
	    if( width == 640 ) { mode = DC1394_VIDEO_MODE_640x480_MONO8; break; }
	    if( width == 800 ) { mode = DC1394_VIDEO_MODE_800x600_MONO8; break; }
	    if( width == 1024) { mode = DC1394_VIDEO_MODE_1024x768_MONO8; break; }
	    if( width == 1280) { mode = DC1394_VIDEO_MODE_1280x960_MONO8; break; }
	    if( width == 1600) { mode = DC1394_VIDEO_MODE_1600x1200_MONO8; break; }
	    goto notfound;
	case MONO16:
	    if( width == 640 ) { mode = DC1394_VIDEO_MODE_640x480_MONO16; break; }
	    if( width == 800 ) { mode = DC1394_VIDEO_MODE_800x600_MONO16; break; }
	    if( width == 1024) { mode = DC1394_VIDEO_MODE_1024x768_MONO16; break; }
	    if( width == 1280) { mode = DC1394_VIDEO_MODE_1280x960_MONO16; break; }
	    if( width == 1600) { mode = DC1394_VIDEO_MODE_1600x1200_MONO16; break; }
	    goto notfound;
#if notyet
	case FORMAT7:
	    { mode = DC1394_VIDEO_MODE_FORMAT7_0; break; }
	    { mode = DC1394_VIDEO_MODE_FORMAT7_1; break; }
	    { mode = DC1394_VIDEO_MODE_FORMAT7_2; break; }
	    { mode = DC1394_VIDEO_MODE_FORMAT7_3; break; }
	    { mode = DC1394_VIDEO_MODE_FORMAT7_4; break; }
	    { mode = DC1394_VIDEO_MODE_FORMAT7_5; break; }
	    { mode = DC1394_VIDEO_MODE_FORMAT7_6; break; }
	    { mode = DC1394_VIDEO_MODE_FORMAT7_7; break; }
	case EXIF:
	    { mode = DC1394_VIDEO_MODE_EXIF; break; }
#endif
	case BGR8:
	case MJPEG:
notfound:
	default:
	    throw std::string("DC1394Camera:setFormat:Unknown/Unsupported Format");
    }
    this->videoMode = mode;

    dc1394error_t result = dc1394_video_set_mode( this->camera, this->videoMode );
    if( result != DC1394_SUCCESS )
	throw std::string("DC1394Camera:setFormat: Failed to set Format");

    Camera::setFormat( format, width, height );

    dc1394_video_set_framerate( this->camera, DC1394_FRAMERATE_15 );
}

void DC1394Camera::setExposureMode( const ExposureMode t )
{
    dc1394error_t retval;
    switch( t ){
	case AUTO_SHUTTER_PRIORITY:
	    retval = dc1394_feature_set_mode( camera, DC1394_FEATURE_IRIS,
					      DC1394_FEATURE_MODE_AUTO );
	    if( retval != DC1394_SUCCESS)
		throw std::string("DC1394Camera:setExposure: Failed to set Auto IRIS");

	    retval = dc1394_feature_set_mode( camera, DC1394_FEATURE_SHUTTER,
					      DC1394_FEATURE_MODE_AUTO );
	    if ( retval != DC1394_SUCCESS )
		throw std::string("DC1394Camera:setExposure: Failed to set AUTO SHUTTEr");

	    break;

	case AUTO_APERTURE_PRIORITY:
	    retval = dc1394_feature_set_mode( camera, DC1394_FEATURE_IRIS,
					      DC1394_FEATURE_MODE_MANUAL );
	    if( retval != DC1394_SUCCESS)
		throw std::string("DC1394Camera:setExposure: Failed to set Manual IRIS");

	    retval = dc1394_feature_set_mode( camera, DC1394_FEATURE_SHUTTER,
					      DC1394_FEATURE_MODE_AUTO );

	    if( retval != DC1394_SUCCESS)
		throw std::string("DC1394Camera:setExposure: Failed to set AUTO Shutter");

	    break;

	case AUTO:
	    retval =dc1394_feature_set_mode( camera, DC1394_FEATURE_EXPOSURE,
					     DC1394_FEATURE_MODE_AUTO );
	    if( retval != DC1394_SUCCESS)
		throw std::string("DC1394Camera:setExposure: Failed to set AUTO EXPOSURE");

	    break;

	case MANUAL:
	    retval = dc1394_feature_set_mode( camera,
					      DC1394_FEATURE_EXPOSURE,
					      DC1394_FEATURE_MODE_MANUAL );
	    if( retval != DC1394_SUCCESS)
		throw std::string("DC1394Camera:setExposure: Failed to set Manual EXPOSURE");
	    break;
    }
}

void DC1394Camera::setBrightness( const int value )
{
	if( value < 0 )
	{
		// set brightness to automatic
		// set birghtness to levels specified.
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_BRIGHTNESS, DC1394_FEATURE_MODE_AUTO ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setBrightness: Failed to set Auto Brightness");
	}
	else
	{
		// set brightness to levels specified.
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_BRIGHTNESS, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setBrightness: Failed to set Manual Brightness");

		// atempt to tweak the brightness :)
		if( dc1394_feature_set_value( camera, DC1394_FEATURE_BRIGHTNESS, value ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setBrightness: Failed to set Manual Brightness to requested value");

	}
}

void DC1394Camera::setControlValue(const Control control, const int value )
{
    switch( control )
    {
	case APERTURE: return this->setIris(value);
	case BRIGHTNESS: return this->setBrightness( value );
	case GAIN: return this->setGain( value );
	case WHITE_BALANCE:
	case CONTRAST:
	case SATURATION:
	case POWER_FREQUENCY:
	case SHARPNESS:
	case EXPOSURE:
	case FRAMERATE:
			 printf("DC1394Camera: setControlValue: Control %d not supported/implemented\n",control);
    }
}


void DC1394Camera::startup()
{
	// setup up the capture and define the number of DMA buffers to use.
	if( dc1394_capture_setup( this->camera, 4, DC1394_CAPTURE_FLAGS_DEFAULT ) != DC1394_SUCCESS )
	    throw std::string("DC1394Camera:startup: Failed to init the DMA buffers for capture");

	//  have the camera start sending us data?
	if( dc1394_video_set_transmission( this->camera, DC1394_ON ) != DC1394_SUCCESS ){
	    this->shutdown();
	    throw std::string("DC1394Camera:startup: Failed to start iso transmition starting (ie camera capture)");
	}

	this->running = true;
}


// Release the camera.
void DC1394Camera::shutdown( void )
{
	// We don't throw exceptions on these as
	// we might be trying to recover from an eariler error
	dc1394_video_set_transmission( this->camera, DC1394_OFF );
	dc1394_capture_stop( this->camera );

	this->running=false;
}

// method to get a frame from the camera
const unsigned char* DC1394Camera::getFrame()
{
    dc1394error_t result;

    if(!this->running)
	this->startup();

    // check if there is a valid frame left over in the buffer
    if( this->frame != NULL )
    {
	result = dc1394_capture_enqueue( this->camera, this->frame );
	if( result != DC1394_SUCCESS )
	    throw std::string("DC1394Camera:getFrame:Failed to remove previous captured frame");
    }

    // capture one frame
    result = dc1394_capture_dequeue( this->camera, DC1394_CAPTURE_POLICY_WAIT, &this->frame );
    if( result != DC1394_SUCCESS )
	    throw std::string("DC1394Camera:getFrame:Failed to capture frame");

    /* XXX - fixup, bayer conversion should live partly in Camera class. - benjsc
     * 20100211
    // check if we need to do a bayer conversion
    if( this->getUseBayer() )
    {
    // attempt to perform the conversion from bayer to rgb
    this->result = dc1394_bayer_decoding_8bit( this->frame->image,
    this->convertedImage, this->width,
    this->height, this->colorFilter,
    this->bayerMethod );

    // check to see if there were any errors.
    if( this->result != DC1394_SUCCESS )
    {
    this->shutdown();

    fprintf( stderr, "failed to convert to bayer, libdc1394 said: %s", dc1394_error_get_string( this->result ) );
    exit( EXIT_FAILURE );
    }

    return this->convertedImage;
    }
    */

    return frame->image;
}


/*
// method to change to the framerate specified by the string.
void DC1394Camera::setFramerate( char* newFramerate )
{
	// make sure the video mode wasn't null
	if( newFramerate != NULL )
	{

		// check to see what the user passed in
		if( strcmp( newFramerate, "DC1394_FRAMERATE_1_875" ) == 0 )
		{
			this->framerate = DC1394_FRAMERATE_1_875;
		}
		else if( strcmp( newFramerate, "DC1394_FRAMERATE_3_75" ) == 0 )
		{
			this->framerate = DC1394_FRAMERATE_3_75;
		}
		else if( strcmp( newFramerate, "DC1394_FRAMERATE_7_5" ) == 0 )
		{
			this->framerate = DC1394_FRAMERATE_7_5;
		}
		else if( strcmp( newFramerate, "DC1394_FRAMERATE_15" ) == 0 )
		{
			this->framerate = DC1394_FRAMERATE_15;
		}
		else if( strcmp( newFramerate, "DC1394_FRAMERATE_30" ) == 0 )
		{
			this->framerate = DC1394_FRAMERATE_30;
		}
		else if( strcmp( newFramerate, "DC1394_FRAMERATE_60" ) == 0 )
		{
			this->framerate = DC1394_FRAMERATE_60;
		}
		else if( strcmp( newFramerate, "DC1394_FRAMERATE_120" ) == 0 )
		{
			this->framerate = DC1394_FRAMERATE_120;
		}
		else if( strcmp( newFramerate, "DC1394_FRAMERATE_240" ) == 0 )
		{
			this->framerate = DC1394_FRAMERATE_240;
		}
		else
		{
			fprintf( stderr, "Framerate not supported" );
			this->printSupportedFramerates();
			fprintf( stderr, "unsupported framereat, see list above for available framerates" );
			exit( EXIT_FAILURE );
		}

		// check that the framerate is supported
		if( !isFramerateSupported( this->framerate ) )
		{
			fprintf( stderr, "failed to set the framerate" );
			this->printSupportedFramerates();
			fprintf( stderr, "Please check that the framerate you requested is supported by the camera (see list above)" );
			exit( EXIT_FAILURE );
		}

		// make sure the camera is set up
		if( this->camera == NULL )
		{
			fprintf( stderr, "camera isn't initialized" );
			exit( EXIT_FAILURE );
		}

		// attempt to set the framerate
		this->result = dc1394_video_set_framerate( this->camera, this->framerate );

		// check if everything went okay
		if( this->result != DC1394_SUCCESS )
		{
			fprintf( stderr, "failed to set the framerate" );
			this->printSupportedVideoModes();
			fprintf( stderr, "The error that I got from libdc1394 was %s", dc1394_error_get_string( this->result ) );
			exit( EXIT_FAILURE );
		}
		else
		{
			fprintf( stderr, "set the framerate to %s", newFramerate );
		}

	}
}
*/

/*
// method to change to the ISO speed
void DC1394Camera::setISOSpeed( char* newISOSpeed )
{
	dc1394speed_t ISOSpeed;

	if( strcmp( newISOSpeed, "DC1394_ISO_SPEED_100" ) == 0 )
	{
		ISOSpeed = DC1394_ISO_SPEED_100;
	}
	else if( strcmp( newISOSpeed, "DC1394_ISO_SPEED_200" ) == 0 )
	{
		ISOSpeed = DC1394_ISO_SPEED_200;
	}
	else if( strcmp( newISOSpeed, "DC1394_ISO_SPEED_400" ) == 0 )
	{
		ISOSpeed = DC1394_ISO_SPEED_400;
	}
	else if( strcmp( newISOSpeed, "DC1394_ISO_SPEED_800" ) == 0 )
	{
		ISOSpeed = DC1394_ISO_SPEED_800;
	}
	else if( strcmp( newISOSpeed, "DC1394_ISO_SPEED_1600" ) == 0 )
	{
		ISOSpeed = DC1394_ISO_SPEED_1600;
	}
	else if( strcmp( newISOSpeed, "DC1394_ISO_SPEED_3200" ) == 0 )
	{
		ISOSpeed = DC1394_ISO_SPEED_3200;
	}
	else
	{
		fprintf( stderr, "ISO speed not supported" );
		//this->printSupportedISOSpeeds();
		fprintf( stderr, "unsupported ISO speed, see list above for available speeds" );
		exit( EXIT_FAILURE );
	}

	// make sure the camera is set up
	if( this->camera == NULL )
	{
		fprintf( stderr, "camera isn't initialized" );
		exit( EXIT_FAILURE );
	}

	fprintf( stderr, "about to set ISO speed to: %s", newISOSpeed );

	this->result = dc1394_video_set_iso_speed( this->camera, ISOSpeed );
	if( this->result != DC1394_SUCCESS )
	{
		fprintf( stderr, "failed to set ISO speed to %s", newISOSpeed );
		fprintf( stderr, "currently I have no way of checking was the available speeds are" );
		fprintf( stderr, "error message is: %s", dc1394_error_get_string( this->result ) );
		exit( EXIT_FAILURE );
	}

	fprintf( stderr, "ISO speed has been set to: %s", newISOSpeed );
}
*/


void DC1394Camera::setIris( const int value)
{
	if( value <0 )
	{
		// set gain to automatic
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_IRIS, DC1394_FEATURE_MODE_AUTO ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setIris: Failed to set IRIS to auto mode");
	}
	else
	{
		// set birghtness to levels specified.
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_IRIS, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setIris: Failed to set IRIS to manual mode");

		// atempt to tweak the gain :)
		if( dc1394_feature_set_value( camera, DC1394_FEATURE_IRIS, value ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setIris: Failed to set IRIS to requested value");
	}

}

void DC1394Camera::setGain( const int gain)
{
	if( gain <0 )
	{
		// set gain to automatic
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setIris: Failed to set gain to auto mode");
	}
	else
	{
		// set birghtness to levels specified.
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setIris: Failed to set gain to manual mode");

		// atempt to tweak the gain :)
		if( dc1394_feature_set_value( camera, DC1394_FEATURE_GAIN, gain ) != DC1394_SUCCESS )
		    throw std::string("DC1394Camera:setIris: Failed to set gain to requested value");
	}
}

/*
// tell the camera that it is to work in trigger mode
void DC1394Camera::useTrigger( bool turnOn )
{
	dc1394switch_t mode = DC1394_OFF;

	this->result = dc1394_external_trigger_set_mode( this->camera,
			DC1394_TRIGGER_MODE_0 );

	if( this->result != DC1394_SUCCESS )
	{
		fprintf( stderr, "Unable to set the trigger mode, libdc said: %s",
				dc1394_error_get_string( this->result ) );
		exit( EXIT_FAILURE );
	}

	// check if we actually want to turn trigger mode on or not
	if( turnOn )
	{
		mode = DC1394_ON;
	}

	// attempt to turn on the trigger mode
	this->result = dc1394_external_trigger_set_power( this->camera, mode ); 

	// check if everything went okay
	if( this->result != DC1394_SUCCESS )
	{
		fprintf( stderr, "unable to set the trigger mode to manual, libdc's error was: %s", dc1394_error_get_string( this->result ) );
	}

}
*/

/*
// find out what sort of conversion method to use.
void DC1394Camera::setBayerMethod( char* bayerMethod )
{
	// check if the bayer method to use is valid
	if( bayerMethod == NULL )
	{
		fprintf( stderr, "The bayer method to use has not been defined" );
		exit( EXIT_FAILURE );
	}

	if( strcmp( bayerMethod, "DC1394_BAYER_METHOD_NEAREST" ) == 0 )
	{
		this->bayerMethod = DC1394_BAYER_METHOD_NEAREST;
	}
	else if( strcmp( bayerMethod, "DC1394_BAYER_METHOD_SIMPLE" ) == 0 )
	{
		this->bayerMethod = DC1394_BAYER_METHOD_SIMPLE;
	}
	else if( strcmp( bayerMethod, "DC1394_BAYER_METHOD_BILINEAR" ) == 0 )
	{
		this->bayerMethod = DC1394_BAYER_METHOD_BILINEAR;
	}
	else if( strcmp( bayerMethod, "DC1394_BAYER_METHOD_HQLINEAR" ) == 0 )
	{
		this->bayerMethod = DC1394_BAYER_METHOD_HQLINEAR;
	}
	else if( strcmp( bayerMethod, "DC1394_BAYER_METHOD_DOWNSAMPLE" ) == 0 )
	{
		this->bayerMethod = DC1394_BAYER_METHOD_DOWNSAMPLE;
	}
	else if( strcmp( bayerMethod, "DC1394_BAYER_METHOD_EDGESENSE" ) == 0 )
	{
		this->bayerMethod = DC1394_BAYER_METHOD_EDGESENSE;
	}
	else if( strcmp( bayerMethod, "DC1394_BAYER_METHOD_VNG" ) == 0 )
	{
		this->bayerMethod = DC1394_BAYER_METHOD_VNG;
	}
	else if( strcmp( bayerMethod, "DC1394_BAYER_METHOD_AHD" ) == 0 )
	{
		this->bayerMethod = DC1394_BAYER_METHOD_AHD;
	}
	else
	{
		this->shutdown();

		fprintf( stderr, "Bayer method is not defined, check out conversions.h for valid bayer modes" );
		exit( EXIT_FAILURE );
	}

	fprintf( stderr, "Will be using bayer method: %s", bayerMethod );
}
// find out what sort of bayer color filter to use
void DC1394Camera::setColorFilter( char* colorFilter )
{
	// check to see if the color filter was valid
	if( colorFilter == NULL )
	{
		this->shutdown();
		fprintf( stderr, "the bayer color filter is was not defined" );
		exit( EXIT_FAILURE );
	}

	// find out what color filter to use
	if( strcmp( colorFilter, "DC1394_COLOR_FILTER_RGGB" ) == 0 )
	{
		this->colorFilter = DC1394_COLOR_FILTER_RGGB;
	}
	else if( strcmp( colorFilter, "DC1394_COLOR_FILTER_GBRG" ) == 0 )
	{
		this->colorFilter = DC1394_COLOR_FILTER_GBRG;
	}
	else if( strcmp( colorFilter, "DC1394_COLOR_FILTER_GRBG" ) == 0 )
	{
		this->colorFilter = DC1394_COLOR_FILTER_GRBG;
	}
	else if( strcmp( colorFilter, "DC1394_COLOR_FILTER_BGGR" ) == 0 )
	{
		this->colorFilter = DC1394_COLOR_FILTER_BGGR;
	}
	else
	{
		this->shutdown();
		fprintf( stderr, "color filter was not valid, have a look in the control.h file to see the valid color filters." );
		exit( EXIT_FAILURE );
	}

	fprintf( stderr, "Will be using color filter: %s", colorFilter );
}
*/

void DC1394Camera::getCurrentConfig()
{
    dc1394video_mode_t mode;
    dc1394_video_get_mode(this->camera, &mode);

    // Map the libdc mode back to the local setup
    switch( mode )
    {
	case DC1394_VIDEO_MODE_320x240_YUV422:  Camera::setFormat(YUYV422, 320, 240 ); break;
	case DC1394_VIDEO_MODE_640x480_YUV411:  Camera::setFormat(YUYV411, 640, 480); break;
	case DC1394_VIDEO_MODE_640x480_YUV422:  Camera::setFormat(YUYV422, 640, 480); break;
	case DC1394_VIDEO_MODE_640x480_RGB8:    Camera::setFormat(RGB8, 640, 480); break;
	case DC1394_VIDEO_MODE_640x480_MONO8:   Camera::setFormat(MONO8,640, 480); break;
	case DC1394_VIDEO_MODE_640x480_MONO16:  Camera::setFormat(MONO16, 640, 480); break;
	case DC1394_VIDEO_MODE_800x600_YUV422:  Camera::setFormat(YUYV422, 800, 600); break;
	case DC1394_VIDEO_MODE_800x600_RGB8:    Camera::setFormat(RGB8, 800, 600); break;
	case DC1394_VIDEO_MODE_800x600_MONO8:   Camera::setFormat(MONO8, 800, 600); break;
	case DC1394_VIDEO_MODE_1024x768_YUV422: Camera::setFormat(YUYV422, 1024, 768); break;
	case DC1394_VIDEO_MODE_1024x768_RGB8:   Camera::setFormat(RGB8, 1024, 768); break;
	case DC1394_VIDEO_MODE_1024x768_MONO8:  Camera::setFormat(MONO8, 1024, 786); break;
	case DC1394_VIDEO_MODE_800x600_MONO16:  Camera::setFormat(MONO16, 800, 600); break;
	case DC1394_VIDEO_MODE_1024x768_MONO16: Camera::setFormat(MONO16, 1024, 768); break;
	case DC1394_VIDEO_MODE_1280x960_YUV422: Camera::setFormat(YUYV422, 1280, 960); break;
	case DC1394_VIDEO_MODE_1280x960_RGB8:   Camera::setFormat(RGB8, 1280, 960); break;
	case DC1394_VIDEO_MODE_1280x960_MONO8:  Camera::setFormat(MONO8, 1280, 960); break;
	case DC1394_VIDEO_MODE_1600x1200_YUV422:Camera::setFormat(YUYV422, 1600, 1200);break;
	case DC1394_VIDEO_MODE_1600x1200_RGB8:  Camera::setFormat(RGB8, 1600, 1200);break;
	case DC1394_VIDEO_MODE_1600x1200_MONO8: Camera::setFormat(MONO8, 1600, 1200);break;
	case DC1394_VIDEO_MODE_1280x960_MONO16: Camera::setFormat(MONO16, 1280, 960);break;
	case DC1394_VIDEO_MODE_1600x1200_MONO16:Camera::setFormat(MONO16, 1600, 1200);break;
#if 0
	case DC1394_VIDEO_MODE_EXIF:
	case DC1394_VIDEO_MODE_160x120_YUV444:
	case DC1394_VIDEO_MODE_FORMAT7_0:
	case DC1394_VIDEO_MODE_FORMAT7_1:
	case DC1394_VIDEO_MODE_FORMAT7_2:
	case DC1394_VIDEO_MODE_FORMAT7_3:
	case DC1394_VIDEO_MODE_FORMAT7_4:
	case DC1394_VIDEO_MODE_FORMAT7_5:
	case DC1394_VIDEO_MODE_FORMAT7_6:
	case DC1394_VIDEO_MODE_FORMAT7_7:
#endif
    }
}

void DC1394Camera::printDetails()
{
    dc1394video_modes_t videoModes;
    dc1394framerates_t framerates;
    dc1394featureset_t features;

    dc1394_video_get_supported_modes( camera, &videoModes );

    fprintf( stderr, "1394 Camera GUID: %lu\n", this->guid);
    fprintf( stderr, "available video modes:\n" );

    for( int i = 0; i < videoModes.num; i++ )
    {
	switch( videoModes.modes[ i ] )
	{
	    case DC1394_VIDEO_MODE_160x120_YUV444:
		fprintf( stderr, "DC1394_VIDEO_MODE_160X120_YUV444\n" );
		break;
	    case DC1394_VIDEO_MODE_320x240_YUV422:
		fprintf( stderr, "DC1394_VIDEO_MODE_320x240_YUV422\n" );
		break;
	    case DC1394_VIDEO_MODE_640x480_YUV411:
		fprintf( stderr, "DC1394_VIDEO_MODE_640x480_YUV411\n" );
		break;
	    case DC1394_VIDEO_MODE_640x480_YUV422:
		fprintf( stderr, "DC1394_VIDEO_MODE_640x480_YUV422\n" );
		break;
	    case DC1394_VIDEO_MODE_640x480_RGB8:
		fprintf( stderr, "DC1394_VIDEO_MODE_640x480_RGB8\n" );
		break;
	    case DC1394_VIDEO_MODE_640x480_MONO8:
		fprintf( stderr, "DC1394_VIDEO_MODE_640x480_MONO8\n" );
		break;
	    case DC1394_VIDEO_MODE_640x480_MONO16:
		fprintf( stderr, "DC1394_VIDEO_MODE_640x480_MONO16\n" );
		break;
	    case DC1394_VIDEO_MODE_800x600_YUV422:
		fprintf( stderr, "DC1394_VIDEO_MODE_800x600_YUV422\n" );
		break;
	    case DC1394_VIDEO_MODE_800x600_RGB8:
		fprintf( stderr, "DC1394_VIDEO_MODE_800x600_RGB8\n" );
		break;
	    case DC1394_VIDEO_MODE_800x600_MONO8:
		fprintf( stderr, "DC1394_VIDEO_MODE_800x600_MONO8\n" );
		break;
	    case DC1394_VIDEO_MODE_1024x768_YUV422:
		fprintf( stderr, "DC1394_VIDEO_MODE_1024x768_YUV422\n" );
		break;
	    case DC1394_VIDEO_MODE_1024x768_RGB8:
		fprintf( stderr, "DC1394_VIDEO_MODE_1024x768_RGB8\n" );
		break;
	    case DC1394_VIDEO_MODE_1024x768_MONO8:
		fprintf( stderr, "DC1394_VIDEO_MODE_1024x768_MONO8\n" );
		break;
	    case DC1394_VIDEO_MODE_800x600_MONO16:
		fprintf( stderr, "DC1394_VIDEO_MODE_800x600_MONO16\n" );
		break;
	    case DC1394_VIDEO_MODE_1024x768_MONO16:
		fprintf( stderr, "DC1394_VIDEO_MODE_1024x768_MONO16\n" );
		break;
	    case DC1394_VIDEO_MODE_1280x960_YUV422:
		fprintf( stderr, "DC1394_VIDEO_MODE_1280x960_YUV422\n" );
		break;
	    case DC1394_VIDEO_MODE_1280x960_RGB8:
		fprintf( stderr, "DC1394_VIDEO_MODE_1280x960_RGB8\n" );
		break;
	    case DC1394_VIDEO_MODE_1280x960_MONO8:
		fprintf( stderr, "DC1394_VIDEO_MODE_1280x960_MONO8\n" );
		break;
	    case DC1394_VIDEO_MODE_1600x1200_YUV422:
		fprintf( stderr, "DC1394_VIDEO_MODE_1600x1200_YUV422\n" );
		break;
	    case DC1394_VIDEO_MODE_1600x1200_RGB8:
		fprintf( stderr, "DC1394_VIDEO_MODE_1600x1200_RGB8\n" );
		break;
	    case DC1394_VIDEO_MODE_1600x1200_MONO8:
		fprintf( stderr, "DC1394_VIDEO_MODE_1600x1200_MONO8\n" );
		break;
	    case DC1394_VIDEO_MODE_1280x960_MONO16:
		fprintf( stderr, "DC1394_VIDEO_MODE_1280x960_MONO16\n" );
		break;
	    case DC1394_VIDEO_MODE_1600x1200_MONO16:
		fprintf( stderr, "DC1394_VIDEO_MODE_1600x1200_MONO16\n" );
		break;
	    case DC1394_VIDEO_MODE_EXIF:
		fprintf( stderr, "DC1394_VIDEO_MODE_EXIF\n" );
		break;
	    case DC1394_VIDEO_MODE_FORMAT7_0:
		fprintf( stderr, "DC1394_VIDEO_MODE_FORMAT7_0\n" );
		break;
	    case DC1394_VIDEO_MODE_FORMAT7_1:
		fprintf( stderr, "DC1394_VIDEO_MODE_FORMAT7_1\n" );
		break;
	    case DC1394_VIDEO_MODE_FORMAT7_2:
		fprintf( stderr, "DC1394_VIDEO_MODE_FORMAT7_2\n" );
		break;
	    case DC1394_VIDEO_MODE_FORMAT7_3:
		fprintf( stderr, "DC1394_VIDEO_MODE_FORMAT7_3\n" );
		break;
	    case DC1394_VIDEO_MODE_FORMAT7_4:
		fprintf( stderr, "DC1394_VIDEO_MODE_FORMAT7_4\n" );
		break;
	    case DC1394_VIDEO_MODE_FORMAT7_5:
		fprintf( stderr, "DC1394_VIDEO_MODE_FORMAT7_5\n" );
		break;
	    case DC1394_VIDEO_MODE_FORMAT7_6:
		fprintf( stderr, "DC1394_VIDEO_MODE_FORMAT7_6\n" );
		break;
	    case DC1394_VIDEO_MODE_FORMAT7_7:
		fprintf( stderr, "DC1394_VIDEO_MODE_FORMAT7_7\n" );
		break;

	    default:
		fprintf( stderr, "Unknown video format\n" );
	}

	dc1394_video_get_supported_framerates( this->camera, videoModes.modes[i], &framerates );

	// loop through all of the framerates and print them as we come to them.
	for( uint32_t i = 0; i <= framerates.num - 1; i++ )
	{
	    switch( framerates.framerates[ i ] )
	    {
		case DC1394_FRAMERATE_1_875:
		    fprintf( stderr, "\tDC1394_FRAMERATE_1_875\n" );
		    break;
		case DC1394_FRAMERATE_3_75:
		    fprintf( stderr, "\tDC1394_FRAMERATE_3_75\n" );
		    break;
		case DC1394_FRAMERATE_7_5:
		    fprintf( stderr, "\tDC1394_FRAMERATE_7_5\n" );
		    break;
		case DC1394_FRAMERATE_15:
		    fprintf( stderr, "\tDC1394_FRAMERATE_15\n" );
		    break;
		case DC1394_FRAMERATE_30:
		    fprintf( stderr, "\tDC1394_FRAMERATE_30\n" );
		    break;
		case DC1394_FRAMERATE_60:
		    fprintf( stderr, "\tDC1394_FRAMERATE_60\n" );
		    break;
		case DC1394_FRAMERATE_120:
		    fprintf( stderr, "\tDC1394_FRAMERATE_120\n" );
		    break;
		case DC1394_FRAMERATE_240:
		    fprintf( stderr, "\tDC1394_FRAMERATE_240\n" );
		    break;
		default:
		    fprintf( stderr, "Unknown video format\n" );
	    }
	}
    }


    // grab the set of features from the camera.
    dc1394_feature_get_all( this->camera, &features );

    // display the features
    dc1394_feature_print_all(&features, stderr );
}


}
