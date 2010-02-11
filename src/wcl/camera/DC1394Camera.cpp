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

DC1394Camera::DC1394Camera(int guid):
    running(false)
{
	// initialize some of the variables
	this->frame = NULL;
	this->cameras=NULL;

	this->setUseBayer( false );
	this->convertedImage = NULL;

	fprintf( stderr, "about to sleep, just in case a bus reset is in progress" );

	// sleep just a bit. If there was an unclean exit and this program
	// is run too soon after, the bus will not have finished reseting and some
	// strange behaviour can be seen
	sleep( 2 );

	fprintf( stderr, "finished sleeping" );

	// attempt to located the cameras on the firewire bus
	dc1394error_t err;
	dc1394_t * d;
        dc1394camera_list_t * list;
	d = dc1394_new ();

	/* Initialize libdc1394 */
	err=dc1394_camera_enumerate (d, &list);

	/* Find cameras */
	DC1394_ERR(err,"Failed to enumerate cameras");

	if (list->num == 0)
	{
	    /* Verify that we have at least one camera */
	    dc1394_log_error("No cameras found");
	    exit( EXIT_FAILURE );
	}

	// Use first camera or requested guid
	if(guid==-1){
	    camera = dc1394_camera_new (d, list->ids[0].guid);
	    /* Work with first camera */
	    if (!camera)
	    {
		dc1394_log_error("Failed to initialize camera with guid %llx", list->ids[0].guid);

		exit( EXIT_FAILURE );
	    }
	} else {
	    camera = dc1394_camera_new (d, list->ids[0].guid);
	    /* Work with first camera */
	    if (!camera)
	    {
		dc1394_log_error("Failed to initialize camera with guid %llx", list->ids[0].guid);

		exit( EXIT_FAILURE );
	    }
	}

	dc1394_camera_free_list (list);

	fprintf( stderr, "Reseting bus...\n" );
	dc1394_reset_bus (camera);

	//	dc1394error_t result = dc1394_find_cameras( &this->cameras, &this->numCameras );
	/*
	// make sure we found some cameras.
	if( result != DC1394_SUCCESS )
	{
	fprintf( stderr, "unable to located cameras, libdc said: %s", dc1394_error_strings[ result ] );
	exit( EXIT_FAILURE );
	}

	// get the camera nodes and describe them as we find them
	if( this->numCameras < 1 )
	{
	fprintf( stderr, "no cameras found :(" );
	exit( EXIT_FAILURE );
	}

	// grab a pointer to the first camera.
	this->camera = this->cameras[0];

	fprintf( stderr, "working with the first camera on the bus" );

	// free the other cameras
	for( uint32_t i = 1; i < this->numCameras; i++ )
	{
	dc1394_free_camera( this->cameras[ i ] );
	}

	// free the memory associated with the cameras array
	free( this->cameras );
	 */
}

DC1394Camera::~DC1394Camera()
{
	this->shutdown ();
}

void DC1394Camera::setFormat( const ImageFormat format, const unsigned width,
			const unsigned height )
{
    dc1394video_mode_t mode = DC1394_VIDEO_MODE_640x480_RGB8; // Default

#warning DC1349:setFormat This methods needs work.. benjsc - 20100205
    //XXX This needs to be redone
    switch( format ){
	case BGR:
	case MJPEG:
	    // NOT Supported
	case YUYV:
	    if( width == 160 ) mode = DC1394_VIDEO_MODE_160x120_YUV444; break;
	    if( width == 320 ) mode = DC1394_VIDEO_MODE_320x240_YUV422; break;
	    if( width == 640 ) mode = DC1394_VIDEO_MODE_640x480_YUV422; break;
	    //if( width == 640 ) mode = DC1394_VIDEO_MODE_640x480_YUV411; break;
	    if( width == 800 ) mode = DC1394_VIDEO_MODE_800x600_YUV422; break;
	    if( width == 1024 ) mode = DC1394_VIDEO_MODE_1024x768_YUV422; break;
	    if( width == 1280 ) mode = DC1394_VIDEO_MODE_1280x960_YUV422; break;
	    if( width == 1600) mode = DC1394_VIDEO_MODE_1600x1200_YUV422; break;
	case RGB:
	    if( width == 640 ) mode = DC1394_VIDEO_MODE_640x480_RGB8; break;
	    if( width == 800 ) mode = DC1394_VIDEO_MODE_800x600_RGB8; break;
	    if( width == 1024 ) mode = DC1394_VIDEO_MODE_1024x768_RGB8; break;
	    if( width == 1280 ) mode = DC1394_VIDEO_MODE_1280x960_RGB8; break;
	    if( width == 1600) mode = DC1394_VIDEO_MODE_1600x1200_RGB8; break;
#if notyet
	case MONO8:
	    if( width == 640 ) mode = DC1394_VIDEO_MODE_640x480_MONO8; break;
	    if( width == 800 ) mode = DC1394_VIDEO_MODE_800x600_MONO8; break;
	    if( width == 1024) mode = DC1394_VIDEO_MODE_1024x768_MONO8; break;
	    if( width == 1280) mode = DC1394_VIDEO_MODE_1280x960_MONO8; break;
	    if( width == 1600) mode = DC1394_VIDEO_MODE_1600x1200_MONO8; break;
	case MONO16:
	    if( width == 640 ) mode = DC1394_VIDEO_MODE_640x480_MONO16; break;
	    if( width == 800 ) mode = DC1394_VIDEO_MODE_800x600_MONO16; break;
	    if( width == 1024) mode = DC1394_VIDEO_MODE_1024x768_MONO16; break;
	    if( width == 1280) mode = DC1394_VIDEO_MODE_1280x960_MONO16; break;
	    if( width == 1600) mode = DC1394_VIDEO_MODE_1600x1200_MONO16; break;
	case FORMAT7:
	    mode = DC1394_VIDEO_MODE_FORMAT7_0; break;
	    mode = DC1394_VIDEO_MODE_FORMAT7_1; break;
	    mode = DC1394_VIDEO_MODE_FORMAT7_2; break;
	    mode = DC1394_VIDEO_MODE_FORMAT7_3; break;
	    mode = DC1394_VIDEO_MODE_FORMAT7_4; break;
	    mode = DC1394_VIDEO_MODE_FORMAT7_5; break;
	    mode = DC1394_VIDEO_MODE_FORMAT7_6; break;
	    mode = DC1394_VIDEO_MODE_FORMAT7_7; break;
	case EXIF:
	    mode = DC1394_VIDEO_MODE_EXIF; break;
#endif
    }
    this->videoMode = mode;

    // make sure the camera is set up
    if( this->camera == NULL )
    {
	fprintf( stderr, "camera isn't initialized" );
	exit( EXIT_FAILURE );
    }

    // attempt to set the video mode
    this->result = dc1394_video_set_mode( this->camera, this->videoMode );

    // attempt to set the video mode
    if( this->result != DC1394_SUCCESS )
    {
	fprintf( stderr, "failed to set video mode" );
	this->printSupportedVideoModes();
	fprintf( stderr, "message from libdc1394 was: %s", dc1394_error_get_string( this->result ) );
	exit( EXIT_FAILURE );
    }

    // attempt to fetch the image properties now that we know the video mode.
    this->fetchImageProperties();
}

bool DC1394Camera::setExposureMode( const ExposureMode t )
{
    int retval;
    switch( t ){
	case AUTO_SHUTTER_PRIORITY:
	case AUTO_APERTURE_PRIORITY:
	    /* FALL THROUGH */
	    printf("DC1394Camera: AUTO_SHUTTER_PRIORITY & AUTO_APETURE_PRIORITY Not Supported, defaulting to AUTO\n");
	case AUTO:
	    retval =dc1394_feature_set_mode( camera, DC1394_FEATURE_EXPOSURE,
					     DC1394_FEATURE_MODE_AUTO );
	    return ( retval == DC1394_SUCCESS );

	case MANUAL:
	    retval = dc1394_feature_set_mode( camera,
					      DC1394_FEATURE_EXPOSURE,
					      DC1394_FEATURE_MODE_MANUAL );
	    return (retval == DC1394_SUCCESS );
    }
}

void DC1394Camera::setBrightness( char* brightness )
{
	if( brightness == NULL )
	{
		// set brightness to automatic
		// set birghtness to levels specified.
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_BRIGHTNESS, DC1394_FEATURE_MODE_AUTO ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to set the brightness to auto" );
			exit( EXIT_FAILURE );
		}
	}
	else
	{
		// set brightness to levels specified.
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_BRIGHTNESS, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to set the brightness to manual" );
			exit( EXIT_FAILURE );
		}

		fprintf( stderr, "attempting to set brightness to %s", brightness );

		// atempt to tweak the brightness :)
		if( dc1394_feature_set_value( camera, DC1394_FEATURE_BRIGHTNESS, atoi( brightness ) ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to change the brightness of the camera" );
			exit( EXIT_FAILURE );
		}

	}
}

bool DC1394Camera::setControlValue(const Control control, const int value )
{
#warning DC1394Camera:setControlValue NOTIMP YET
    return false;
}


void DC1394Camera::startup()
{
	fprintf( stderr, "finished configuring" );

	// setup up the capture and define the number of DMA buffers to use.
	result = dc1394_capture_setup( this->camera, 4, DC1394_CAPTURE_FLAGS_DEFAULT );

	// check to make sure if everything went okay
	if( result != DC1394_SUCCESS )
	{
		this->shutdown();

		fprintf( stderr, "The only reason I have had this error occur so far is because the "
				"ISO speed was not supported so far everything else is check that it is "
				"supported before attempted to apply the mode. I know that the "
				"ISO speed is a valid speed but I don't know if the camera supports it."
				"libdc said: %s", dc1394_error_get_string( result ) );
		exit( EXIT_FAILURE );

	}

	fprintf( stderr, "start transmission" );

	//  have the camera start sending us data?
	if( dc1394_video_set_transmission( this->camera, DC1394_ON ) != DC1394_SUCCESS )
	{
		fprintf( stderr, "unable to start camera iso transmission" );
		this->shutdown();
	}

	fprintf( stderr, "wait transmission" );

	//  Sleep untill the camera has a transmission
	dc1394switch_t status = DC1394_OFF;

	uint32_t i = 0;
	for( ; status == DC1394_OFF && i < 5; i++ )
	{
		usleep( 50000 );

		// attempt to get the transmission
		this->result = dc1394_video_get_transmission( this->camera, &status );

		// check to see if everything went okay
		if( this->result != DC1394_SUCCESS )
		{
			this->shutdown();

			fprintf( stderr, "failed to get a transmission. libdc1394 said: %s", dc1394_error_get_string( this->result ) );
			exit( EXIT_FAILURE );
		}
	}

	// check if we tried to turn it on 5 times without success
	if( i == 5 )
	{
		this->shutdown ();

		fprintf( stderr, "Camera doesn't seem to want to turn on!" );
		exit( EXIT_FAILURE );
	}
}

/*
// configuration method that reads in values from a config file.
void DC1394Camera::configure()
{

this->setISOSpeed( ConfigurationLoader::getProperty( "ISOSpeed" ) );

this->setVideoMode( ConfigurationLoader::getProperty( "videoMode" ) );

fprintf( stderr, "finished setting the video mode" );

this->setFramerate( ConfigurationLoader::getProperty( "framerate" ) );

fprintf( stderr, "attempting to set exposure" );
this->setExposure( ConfigurationLoader::getProperty( "exposure" ) );
fprintf( stderr, "set exposure" );

this->setBrightness( ConfigurationLoader::getProperty( "brightness" ) );

fprintf( stderr, "set bright" );
this->setGain( ConfigurationLoader::getProperty( "gain" ) );

fprintf( stderr, "set gain" );
this->setShutter( ConfigurationLoader::getProperty( "shutter" ) );

fprintf( stderr, "set shutter" );
// initialize the trigger mode
bool useTriggerMode = false;

// check if we are going to use trigger mode or not.
if( strcmp( "true", ConfigurationLoader::getProperty( "useTrigger" ) ) == 0 )
{
useTriggerMode = true;
}

this->useTrigger( useTriggerMode );

// check if we are to use bayer conversion code
if( ConfigurationLoader::pushContext( "convertFromBayer" ) == 0 )
{
// check if we are going to use bayer conversion
this->setUseBayer( true );

// find out what sort of bayer color filter to use
this->setColorFilter( ConfigurationLoader::getProperty( "colorFilter" ) );

// find out what sort of conversion algorithm to use.
this->setBayerMethod( ConfigurationLoader::getProperty( "method" ) );

this->depth = 3;

// allocate some memory for the bayer image.
this->convertedImage = ( unsigned char* )malloc( sizeof( unsigned char ) * this->width * this->height * this->depth );

// check to see if memory was allocated okay
if( this->convertedImage == NULL )
{
this->cleanup( );

fprintf( stderr, "failed to allocated memory for the converted bayer image" );
exit( EXIT_FAILURE );
}

ConfigurationLoader::popContext();
}
else
{
this->depth = 1;
}

// check if we have to resize the image before returning it
if( ConfigurationLoader::pushContext( "convertSize" ) == 0 )
{
	this->convertWidth = atoi( ConfigurationLoader::getProperty( "width" ) );
	this->convertHeight = atoi( ConfigurationLoader::getProperty( "height" ) );

	ConfigurationLoader::popContext();

	this->resizeWrapper = cvCreateImage( cvSize( this->width, this->height ), IPL_DEPTH_8U, 3 );

	this->finalImage = cvCreateImage( cvSize( this->convertWidth, this->convertHeight ), IPL_DEPTH_8U, 3 );

	this->convertSize = true;
}
else
{
	this->convertWidth = this->width;
	this->convertHeight = this->height;

	this->resizeWrapper = NULL;
	this->convertSize = false;
}
}
*/

// Release the camera.
void DC1394Camera::shutdown( void )
{
	dc1394_video_set_transmission( this->camera, DC1394_OFF );
	dc1394_capture_stop( this->camera );
	dc1394_camera_free( this->camera );

	// attempt to free the memory allocated for the converted Image.
	if( this->convertedImage != NULL )
	{
		delete this->convertedImage;
	}

	this->running=false;
}

// method to get a frame from the camera
const unsigned char* DC1394Camera::getFrame()
{
    if(!this->running)
	this->startup();

	// check if there is a valid frame left over in the buffer
	if( this->frame != NULL )
	{
		// attempt to clear the frame out of the buffer.
		this->result = dc1394_capture_enqueue( this->camera, this->frame );

		// check to see if there were any errors
		if( this->result != DC1394_SUCCESS )
		{
			fprintf( stderr, "failed to clear the buffer. libdc1394 said: %s", dc1394_error_get_string( this->result ) );
		}
	}

	// capture one frame
	this->result = dc1394_capture_dequeue( this->camera, DC1394_CAPTURE_POLICY_WAIT, &this->frame );

	// check to see if the capturing went okay
	if( this->result != DC1394_SUCCESS )
	{
		fprintf( stderr, "Unable to capture a frame. libdc1394 said [ error: %d ]: %s", this->result, dc1394_error_get_string( this->result ) );
	}

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

	return frame->image;
}

void DC1394Camera::close_camera( void )
{
	fprintf( stderr, "stop transmission" );

	// stop transmission
	if( dc1394_video_set_transmission( camera, DC1394_OFF ) != DC1394_SUCCESS)
	{
		fprintf( stderr, "couldn't stop the camera?" );
	}

	// close the camera down
	this->shutdown();
}

// method to populate video_modes with data
void DC1394Camera::getSupportedVideoModes()
{
	// check to see that the camera has been setup okay
	if( this->camera != NULL )
	{
		// get the best video mode and highest framerate. This can be skipped
		// if you already know which mode/framerate you want...
		// get video modes:
		if( dc1394_video_get_supported_modes( camera, &this->videoModes ) != DC1394_SUCCESS )
		{
			this->shutdown();

			fprintf( stderr, "Can't get video modes" );
			exit( EXIT_FAILURE );
		}
	}
}

// method to populate framerates with data
void DC1394Camera::getSupportedFramerates()
{

	// make sure the camera is valid
	if( this->camera == NULL )
	{
		fprintf( stderr, "camera has not been initialize yet. Need to do that before running this method" );
		exit( EXIT_FAILURE );
	}

	// attempt to fetch the list of available framerates
	this->result = dc1394_video_get_supported_framerates( this->camera, this->videoMode, &this->framerates );

	// check if everything went smoothly
	if( this->result != DC1394_SUCCESS )
	{
		this->shutdown();

		fprintf( stderr, "Unable to fetch the framerates from the camera\n"
				"The reason that dc1394 gave was: %s", dc1394_error_get_string( this->result ) );
		exit( EXIT_FAILURE );
	}
}

// method to check to see if a video mode is supported
bool DC1394Camera::isVideoModeSupported( dc1394video_mode_t videoMode )
{
	// make sure we have the supported video modes
	this->getSupportedVideoModes();

	bool found = false;

	// go through all the modes and see if there is one that matches	
	for( uint32_t i = 0; ( i < this->videoModes.num ) && !found; i++ )
	{
		// check to see if the current mode matches the one passed in
		if( this->videoModes.modes[ i ] == videoMode )
		{
			found = true;
		}
	}

	// return the result of our search
	return found;
}

// method to check to see if a given framerate is supported
bool DC1394Camera::isFramerateSupported( dc1394framerate_t framerate )
{
	// make sure we have the supported framerates
	this->getSupportedFramerates();

	bool found = false;

	// go through all the framerates and see if there is one that matches	
	for( uint32_t i = 0; ( i < this->framerates.num ) && !found; i++ )
	{
		// check to see if the current mode matches the one passed in
		if( this->framerates.framerates[ i ] == framerate )
		{
			found = true;
		}
	}

	// return the result of our search
	return found;
}

// a method to set the video mode given a string representing the video mode.
void DC1394Camera::setVideoMode( char* newVideoMode )
{
}

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

// method to change to the ISO speed
void DC1394Camera::setISOSpeed( char* newISOSpeed )
{
	if( newISOSpeed == NULL )
	{
		fprintf( stderr, "no ISO speed was specified (newISOSpeed was NULL)" );
		exit( EXIT_FAILURE );
	}

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

void DC1394Camera::setShutter( char* shutter )
{
	if( shutter == NULL )
	{
		fprintf( stderr, "No shutter speed was specified, attempting to set to auto" );
		// set shutter to automatic
		// set shutter to manual using the value passed in
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_AUTO ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to set the brightness to auto" );
			exit( EXIT_FAILURE );
		}
	}
	else
	{
		// set shutter to manual using the value passed in
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to set the brightness to manual" );
			exit( EXIT_FAILURE );
		}

		fprintf( stderr, "attempting to set shutter to %s", shutter );

		// atempt to tweak the shutter :)
		if( dc1394_feature_set_value( camera, DC1394_FEATURE_SHUTTER, atoi( shutter ) ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to change the brightness of the camera" );
			exit( EXIT_FAILURE );
		}
	}	
}


void DC1394Camera::setGain( char* gain )
{
	if( gain == NULL )
	{
		// set gain to automatic
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to set gain to auto" );
			exit( EXIT_FAILURE );
		}
	}
	else
	{
		// set birghtness to levels specified.
		if( dc1394_feature_set_mode( camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to set the gain to manual" );
			exit( EXIT_FAILURE );
		}

		fprintf( stderr, "attempting to set gain to %s", gain );

		// atempt to tweak the gain :)
		if( dc1394_feature_set_value( camera, DC1394_FEATURE_GAIN, atoi( gain ) ) != DC1394_SUCCESS )
		{
			fprintf( stderr, "unable to change the gain of the camera" );
			exit( EXIT_FAILURE );
		}
	}
}

// method to display to the user the different video modes available
void DC1394Camera::printSupportedVideoModes()
{
	// make sure we have a list of the available videoModes
	this->getSupportedVideoModes();

	fprintf( stderr, "available video modes:" );

	// select highest res mode:
	for( uint32_t i = 0; i <= this->videoModes.num - 1; i++ )
	{
		switch( this->videoModes.modes[ i ] )
		{
			case DC1394_VIDEO_MODE_160x120_YUV444:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_160X120_YUV444" );
				break;
			case DC1394_VIDEO_MODE_320x240_YUV422:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_320x240_YUV422" );
				break;
			case DC1394_VIDEO_MODE_640x480_YUV411:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_640x480_YUV411" );
				break;
			case DC1394_VIDEO_MODE_640x480_YUV422:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_640x480_YUV422" );
				break;
			case DC1394_VIDEO_MODE_640x480_RGB8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_640x480_RGB8" );
				break;
			case DC1394_VIDEO_MODE_640x480_MONO8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_640x480_MONO8" );
				break;
			case DC1394_VIDEO_MODE_640x480_MONO16:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_640x480_MONO16" );
				break;
			case DC1394_VIDEO_MODE_800x600_YUV422:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_800x600_YUV422" );
				break;
			case DC1394_VIDEO_MODE_800x600_RGB8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_800x600_RGB8" );
				break;
			case DC1394_VIDEO_MODE_800x600_MONO8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_800x600_MONO8" );
				break;
			case DC1394_VIDEO_MODE_1024x768_YUV422:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1024x768_YUV422" );
				break;
			case DC1394_VIDEO_MODE_1024x768_RGB8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1024x768_RGB8" );
				break;
			case DC1394_VIDEO_MODE_1024x768_MONO8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1024x768_MONO8" );
				break;
			case DC1394_VIDEO_MODE_800x600_MONO16:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_800x600_MONO16" );
				break;
			case DC1394_VIDEO_MODE_1024x768_MONO16:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1024x768_MONO16" );
				break;
			case DC1394_VIDEO_MODE_1280x960_YUV422:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1280x960_YUV422" );
				break;
			case DC1394_VIDEO_MODE_1280x960_RGB8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1280x960_RGB8" );
				break;
			case DC1394_VIDEO_MODE_1280x960_MONO8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1280x960_MONO8" );
				break;
			case DC1394_VIDEO_MODE_1600x1200_YUV422:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1600x1200_YUV422" );
				break;
			case DC1394_VIDEO_MODE_1600x1200_RGB8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1600x1200_RGB8" );
				break;
			case DC1394_VIDEO_MODE_1600x1200_MONO8:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1600x1200_MONO8" );
				break;
			case DC1394_VIDEO_MODE_1280x960_MONO16:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1280x960_MONO16" );
				break;
			case DC1394_VIDEO_MODE_1600x1200_MONO16:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_1600x1200_MONO16" );
				break;
			case DC1394_VIDEO_MODE_EXIF:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_EXIF" );
				break;
			case DC1394_VIDEO_MODE_FORMAT7_0:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_FORMAT7_0" );
				break;
			case DC1394_VIDEO_MODE_FORMAT7_1:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_FORMAT7_1" );
				break;
			case DC1394_VIDEO_MODE_FORMAT7_2:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_FORMAT7_2" );
				break;
			case DC1394_VIDEO_MODE_FORMAT7_3:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_FORMAT7_3" );
				break;
			case DC1394_VIDEO_MODE_FORMAT7_4:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_FORMAT7_4" );
				break;
			case DC1394_VIDEO_MODE_FORMAT7_5:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_FORMAT7_5" );
				break;
			case DC1394_VIDEO_MODE_FORMAT7_6:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_FORMAT7_6" );
				break;
			case DC1394_VIDEO_MODE_FORMAT7_7:
				fprintf( stderr, "\t\tDC1394_VIDEO_MODE_FORMAT7_7" );
				break;
			default:
				fprintf( stderr, "Unknown video format" );
				exit( EXIT_FAILURE );
		}
	}
}

// method to display to the user the different framerates available
void DC1394Camera::printSupportedFramerates()
{
	// make sure we have a list of the available videoModes
	this->getSupportedFramerates();

	fprintf( stderr, "available framerates:" );

	// loop through all of the framerates and print them as we come to them.
	for( uint32_t i = 0; i <= this->framerates.num - 1; i++ )
	{
		switch( this->framerates.framerates[ i ] )
		{
			case DC1394_FRAMERATE_1_875:
				fprintf( stderr, "\t\tDC1394_FRAMERATE_1_875" );
				break;
			case DC1394_FRAMERATE_3_75:
				fprintf( stderr, "\t\tDC1394_FRAMERATE_3_75" );
				break;
			case DC1394_FRAMERATE_7_5:
				fprintf( stderr, "\t\tDC1394_FRAMERATE_7_5" );
				break;
			case DC1394_FRAMERATE_15:
				fprintf( stderr, "\t\tDC1394_FRAMERATE_15" );
				break;
			case DC1394_FRAMERATE_30:
				fprintf( stderr, "\t\tDC1394_FRAMERATE_30" );
				break;
			case DC1394_FRAMERATE_60:
				fprintf( stderr, "\t\tDC1394_FRAMERATE_60" );
				break;
			case DC1394_FRAMERATE_120:
				fprintf( stderr, "\t\tDC1394_FRAMERATE_120" );
				break;
			case DC1394_FRAMERATE_240:
				fprintf( stderr, "\t\tDC1394_FRAMERATE_240" );
				break;
			default:
				fprintf( stderr, "Unknown video format" );
				exit( EXIT_FAILURE );
		}
	}
}

// display a bunch of info about what the camera is capable of
void DC1394Camera::printFeatureSet()
{
	dc1394featureset_t features;

	// grab the set of features from the camera.
	this->result = dc1394_feature_get_all( this->camera, &features );

	// check if everything went okay
	if( this->result != DC1394_SUCCESS)
	{
		fprintf( stderr, "unable fetch the set of features, lidc said: %s", dc1394_error_get_string( this->result ) );

		exit( EXIT_FAILURE );
	}

	// display the features
	dc1394_feature_print_all(&features, stderr );
}

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

// returns the width of the image we are working with
int DC1394Camera::getWidth()
{
	return this->width;
}

// returns the height of the image we are working with
int DC1394Camera::getHeight()
{
	return this->height;
}
// set the flag that tells us if we need to convert from a bayer image or not
void DC1394Camera::setUseBayer( bool useBayer )
{
	this->useBayer = useBayer;
}

// return whether or not we need to convert from bayer or not
bool DC1394Camera::getUseBayer()
{
	return this->useBayer;
}

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

// fetch the image dimensions based on the camera setup
void DC1394Camera::fetchImageProperties()
{
	// attempt to fetch the image dimensions from the camera.
	this->result = dc1394_get_image_size_from_video_mode( this->camera, this->videoMode, &width, &height );

	// find out if the read was successful
	if( this->result != DC1394_SUCCESS )
	{
		this->shutdown();

		fprintf( stderr, "failed to fetch the image dimensions. libdc1394 said: %s", dc1394_error_get_string( this->result) );
		exit( EXIT_FAILURE );
	}

	fprintf( stderr, "Image is %dx%d", this->width, this->height );
}


void DC1394Camera::printDetails()
{
    this->printSupportedVideoModes();
    this->printSupportedFramerates();
    this->printFeatureSet();
}


}
