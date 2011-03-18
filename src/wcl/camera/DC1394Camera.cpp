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
#include <unistd.h>
#include <string.h>
#include <IO.h>
#include <sstream>
#include <dc1394/dc1394.h>
#include <dc1394/utils.h>

#include "CameraException.h"
#include "DC1394Camera.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

using namespace std;

namespace wcl {

    struct DC1394tolibWCLModes
    {
	dc1394video_mode_t  dc1394mode;
	Camera::ImageFormat libwclmode;
	unsigned		libwclwidth;
	unsigned		libwclheight;

    };

    struct DC1394tolibWCLFPS
    {
	dc1394framerate_t   dc1394fps;
	unsigned		libwclfps;
    };

    static DC1394tolibWCLModes formatConversion[] =
    {
	{ DC1394_VIDEO_MODE_160x120_YUV444, Camera::YUYV444, 160, 120},
	{ DC1394_VIDEO_MODE_320x240_YUV422, Camera:: YUYV422, 320, 240},
	{ DC1394_VIDEO_MODE_640x480_YUV411, Camera::YUYV411, 640, 480},
	{ DC1394_VIDEO_MODE_640x480_YUV422, Camera::YUYV422, 640, 480},
	{ DC1394_VIDEO_MODE_640x480_RGB8, Camera::RGB8, 640, 480},
	{ DC1394_VIDEO_MODE_640x480_MONO8, Camera::MONO8, 640, 480},
	{ DC1394_VIDEO_MODE_640x480_MONO16, Camera::MONO16, 640, 480},
	{ DC1394_VIDEO_MODE_800x600_YUV422, Camera::YUYV422, 800, 600},
	{ DC1394_VIDEO_MODE_800x600_RGB8, Camera::RGB8, 800, 600},
	{ DC1394_VIDEO_MODE_800x600_MONO8, Camera::MONO8, 800, 600},
	{ DC1394_VIDEO_MODE_1024x768_YUV422, Camera::YUYV422, 1024, 768},
	{ DC1394_VIDEO_MODE_1024x768_RGB8, Camera::RGB8, 1024, 768},
	{ DC1394_VIDEO_MODE_1024x768_MONO8, Camera:: MONO8, 1024, 768},
	{ DC1394_VIDEO_MODE_800x600_MONO16, Camera::MONO16, 800, 600},
	{ DC1394_VIDEO_MODE_1024x768_MONO16, Camera::MONO16, 1024, 768},
	{ DC1394_VIDEO_MODE_1280x960_YUV422, Camera::YUYV422, 1280, 960},
	{ DC1394_VIDEO_MODE_1280x960_RGB8, Camera::RGB8, 1280, 960},
	{ DC1394_VIDEO_MODE_1280x960_MONO8, Camera::MONO8, 1280, 960},
	{ DC1394_VIDEO_MODE_1600x1200_YUV422, Camera::YUYV422, 1600, 1200},
	{ DC1394_VIDEO_MODE_1600x1200_RGB8, Camera::RGB8, 1600, 1200},
	{ DC1394_VIDEO_MODE_1600x1200_MONO8, Camera::MONO8, 1600, 1200},
	{ DC1394_VIDEO_MODE_1280x960_MONO16, Camera::MONO16, 1280, 960},
	{ DC1394_VIDEO_MODE_1600x1200_MONO16, Camera::MONO16, 1600, 1200}
#if 0
	case DC1394_VIDEO_MODE_EXIF:
	case DC1394_VIDEO_MODE_FORMAT7_0:
	case DC1394_VIDEO_MODE_FORMAT7_1:
	case DC1394_VIDEO_MODE_FORMAT7_2:
	case DC1394_VIDEO_MODE_FORMAT7_3:
	case DC1394_VIDEO_MODE_FORMAT7_4:
	case DC1394_VIDEO_MODE_FORMAT7_5:
	case DC1394_VIDEO_MODE_FORMAT7_6:
	case DC1394_VIDEO_MODE_FORMAT7_7:
#endif
    };

    static DC1394tolibWCLFPS fpsConversion[] =
    {
	{ DC1394_FRAMERATE_1_875, 1.875 },
	{ DC1394_FRAMERATE_3_75, 3.75 },
	{ DC1394_FRAMERATE_7_5, 7.5 },
	{ DC1394_FRAMERATE_15, 15.0 },
	{ DC1394_FRAMERATE_30, 30.0 },
	{ DC1394_FRAMERATE_60, 60.0 },
	{ DC1394_FRAMERATE_120, 120.0 },
	{ DC1394_FRAMERATE_240, 240.0 },
    };


    DC1394Camera::DC1394Camera(const uint64_t myguid):
	d(NULL),
	frame(NULL),
	guid(myguid),
	running(false)
    {

	//why do we need a GUID and an ID?!
	stringstream ss;
	ss << guid;
	this->id = ss.str();

	this->d = dc1394_new ();
	if( !this->d )
	    throw CameraException(CameraException::EACCESS);
	this->camera = dc1394_camera_new( this->d, myguid );
	if( !this->camera )
	    throw CameraException(CameraException::NOTFOUND);

	// Just a safety thing, 1394 is a little touchy
	dc1394_reset_bus (this->camera);


	this->loadCapabilities();
    }

    DC1394Camera::~DC1394Camera()
    {
	this->shutdown();
	dc1394_camera_free( this->camera );
	dc1394_free(this->d);
    }

    void DC1394Camera::setConfiguration(const Configuration &c)
    {
	dc1394video_mode_t mode;
	dc1394framerate_t rate;
	unsigned i;

	// Find the format requested
	for(i = 0; i < ARRAY_SIZE(formatConversion); i++){
	    if(formatConversion[i].libwclmode == c.format &&
	       formatConversion[i].libwclwidth == c.width &&
	       formatConversion[i].libwclheight == c.height ){
		mode = formatConversion[i].dc1394mode;
		break;
	    }
	}

	if( i == ARRAY_SIZE(formatConversion)){
	    throw CameraException(CameraException::INVALIDFORMAT);
	}


	// Find the requested frame rate
	for(i = 0; i < ARRAY_SIZE(fpsConversion); i++ ) {
	    if( fpsConversion[i].libwclfps == c.fps ){
		rate = fpsConversion[i].dc1394fps;
		break;
	    }
	}

	if( i == ARRAY_SIZE(fpsConversion)){
	    throw CameraException(CameraException::INVALIDFORMAT);
	}

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


	/*
	   On 05/02/10 14:04, Aaron Stafford wrote:
	   err, its all a bit complicated. Does someone need to use this?

	   If you are using format 7 you have to also set the region of interest,
	   so you need to get the desired width and height in pixels from somewhere
	   and then call the appropriate dc1394 functions e.g.:

	   dc1394_format7_set_image_size( this->camera, this->videoMode, width,
	   height );

	   dc1394_format7_set_roi( this->camera, this->videoMode,
	   DC1394_COLOR_CODING_MONO8, DC1394_USE_MAX_AVAIL, 0, 0, width, height );

	   Also if you set FORMAT7_0 or FORMAT7_1 you can't or shouldn't set the
	   frame rate manually as you will not get the desired result, or the
	   camera will just barf or something.

	   The actual code I used is:

	// do not set the framerate if using FORMAT7_0 or FORMAT7_1
	if (this->videoMode != DC1394_VIDEO_MODE_FORMAT7_0 && this->videoMode !=
	DC1394_VIDEO_MODE_FORMAT7_1) {
	this->setFramerate (ConfigurationLoader::getProperty("framerate"));
	}

	...

	// set the region of interest
	if (this->videoMode == DC1394_VIDEO_MODE_FORMAT7_0) {
	int width = atoi (ConfigurationLoader::getProperty("format7_width"));

	int height = atoi
	(ConfigurationLoader::getProperty("format7_height"));

	dc1394_format7_set_image_size (this->camera, this->videoMode,
	width, height);

	dc1394_format7_set_roi (this->camera, this->videoMode,
	DC1394_COLOR_CODING_MONO8, 
	DC1394_USE_MAX_AVAIL, 0, 0, width, height);
	}

	Hope this helps.
	*/

#endif

	// With the modes and framerates worked out we
	// actually try and setup the camera. We don't do this after
	// each step above as if an invalid mode is selected (ie
	// framerate) we want to pick it up before trying to actually 
	// change the camera to that mode.
	dc1394error_t result = dc1394_video_set_mode( this->camera, mode );
	if( result != DC1394_SUCCESS )
	    throw CameraException(CameraException::CONTROLERROR);

	dc1394_video_set_framerate( this->camera, rate );
	if( result != DC1394_SUCCESS )
	    throw CameraException(CameraException::CONTROLERROR);

	Camera::setConfiguration(c);
    }

    void DC1394Camera::setExposureMode( const ExposureMode t )
    {
	dc1394error_t retval;
	switch( t ){
	    case AUTO_SHUTTER_PRIORITY:
		retval = dc1394_feature_set_mode( camera, DC1394_FEATURE_IRIS,
						  DC1394_FEATURE_MODE_AUTO );
		if( retval != DC1394_SUCCESS)
		    throw CameraException(CameraException::EXPOSUREERROR);

		retval = dc1394_feature_set_mode( camera, DC1394_FEATURE_SHUTTER,
						  DC1394_FEATURE_MODE_AUTO );
		if ( retval != DC1394_SUCCESS )
		    throw CameraException(CameraException::EXPOSUREERROR);

		break;

	    case AUTO_APERTURE_PRIORITY:
		retval = dc1394_feature_set_mode( camera, DC1394_FEATURE_IRIS,
						  DC1394_FEATURE_MODE_MANUAL );
		if( retval != DC1394_SUCCESS)
		    throw CameraException(CameraException::EXPOSUREERROR);

		retval = dc1394_feature_set_mode( camera, DC1394_FEATURE_SHUTTER,
						  DC1394_FEATURE_MODE_AUTO );

		if( retval != DC1394_SUCCESS)
		    throw CameraException(CameraException::EXPOSUREERROR);

		break;

	    case AUTO:
		retval =dc1394_feature_set_mode( camera, DC1394_FEATURE_EXPOSURE,
						 DC1394_FEATURE_MODE_AUTO );
		if( retval != DC1394_SUCCESS)
		    throw CameraException(CameraException::EXPOSUREERROR);
		break;

	    case MANUAL:
		retval = dc1394_feature_set_mode( camera,
						  DC1394_FEATURE_EXPOSURE,
						  DC1394_FEATURE_MODE_MANUAL );
		if( retval != DC1394_SUCCESS)
		    throw CameraException(CameraException::EXPOSUREERROR);
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
		throw CameraException(CameraException::BRIGHTNESSERROR);
	}
	else
	{
	    // set brightness to levels specified.
	    if( dc1394_feature_set_mode( camera, DC1394_FEATURE_BRIGHTNESS, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		throw CameraException(CameraException::BRIGHTNESSERROR);

	    // atempt to tweak the brightness :)
	    if( dc1394_feature_set_value( camera, DC1394_FEATURE_BRIGHTNESS, value ) != DC1394_SUCCESS )
		throw CameraException(CameraException::BRIGHTNESSERROR);

	}
    }

    void DC1394Camera::setControlValue(const Control control, const int value )
    {
	switch( control )
	{
	    case APERTURE: this->setIris(value); break;
	    case BRIGHTNESS: this->setBrightness( value ); break;
	    case GAIN: this->setGain( value ); break;
	    case ISO: this->setISOSpeed(value);
	    case WHITE_BALANCE:
	    case CONTRAST:
	    case SATURATION:
	    case POWER_FREQUENCY:
	    case SHARPNESS:
	    case EXPOSURE:
	    case FOCUS:
	    case AUTOFOCUS:
		      throw CameraException(CameraException::CONTROLERROR);
	}
    }


    int DC1394Camera::getControlValue(const Control control) {
	throw CameraException("DC1394 Cameras do not yet support getting control values");
    }


    void DC1394Camera::startup()
    {
	// setup up the capture and define the number of DMA buffers to use.
	if( dc1394_capture_setup( this->camera, 4, DC1394_CAPTURE_FLAGS_DEFAULT ) != DC1394_SUCCESS )
	    throw CameraException(CameraException::BUFFERERROR);

	//  have the camera start sending us data?
	if( dc1394_video_set_transmission( this->camera, DC1394_ON ) != DC1394_SUCCESS ){
	    this->shutdown();
	    throw CameraException(CameraException::BUFFERERROR);
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
    void DC1394Camera::update()
    {
	dc1394error_t result;

	if(!this->running)
	    this->startup();

	// check if there is a valid frame left over in the buffer
	if( this->frame != NULL )
	{
	    result = dc1394_capture_enqueue( this->camera, this->frame );
	    if( result != DC1394_SUCCESS )
		throw CameraException(CameraException::BUFFERERROR);
	}

	// capture one frame
	result = dc1394_capture_dequeue( this->camera, DC1394_CAPTURE_POLICY_WAIT, &this->frame );
	if( result != DC1394_SUCCESS )
	    throw CameraException(CameraException::BUFFERERROR);

	currentFrame = frame->image;
    }



    // method to change to the ISO speed
    void DC1394Camera::setISOSpeed( const int value )
    {
	dc1394speed_t isoSpeed;

	switch(value){
	    case 100: isoSpeed = DC1394_ISO_SPEED_100; break;
	    case 200: isoSpeed = DC1394_ISO_SPEED_200; break;
	    case 400: isoSpeed = DC1394_ISO_SPEED_400; break;
	    case 800: isoSpeed = DC1394_ISO_SPEED_800; break;
	    case 1600: isoSpeed = DC1394_ISO_SPEED_1600; break;
	    case 3200: isoSpeed = DC1394_ISO_SPEED_3200; break;
	    default:
		       throw CameraException(CameraException::ISOERROR);
	}

	dc1394error_t result = dc1394_video_set_iso_speed( this->camera, isoSpeed );
	if( result != DC1394_SUCCESS ) {
	    throw CameraException(CameraException::ISOERROR);
	}

    }

    void DC1394Camera::setIris( const int value)
    {
	if( value <0 )
	{
	    // set gain to automatic
	    if( dc1394_feature_set_mode( camera, DC1394_FEATURE_IRIS, DC1394_FEATURE_MODE_AUTO ) != DC1394_SUCCESS )
		throw CameraException(CameraException::IRISERROR);
	}
	else
	{
	    // set birghtness to levels specified.
	    if( dc1394_feature_set_mode( camera, DC1394_FEATURE_IRIS, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		throw CameraException(CameraException::IRISERROR);

	    // atempt to tweak the gain :)
	    if( dc1394_feature_set_value( camera, DC1394_FEATURE_IRIS, value ) != DC1394_SUCCESS )
		throw CameraException(CameraException::IRISERROR);
	}

    }

    void DC1394Camera::setGain( const int gain)
    {
	if( gain <0 )
	{
	    // set gain to automatic
	    if( dc1394_feature_set_mode( camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO ) != DC1394_SUCCESS )
		throw CameraException(CameraException::GAINERROR);
	}
	else
	{
	    // set birghtness to levels specified.
	    if( dc1394_feature_set_mode( camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL ) != DC1394_SUCCESS )
		throw CameraException(CameraException::GAINERROR);

	    // atempt to tweak the gain :)
	    if( dc1394_feature_set_value( camera, DC1394_FEATURE_GAIN, gain ) != DC1394_SUCCESS )
		throw CameraException(CameraException::GAINERROR);
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

void DC1394Camera::loadCapabilities()
{
    dc1394video_modes_t videoModes;
    dc1394framerates_t framerates;
    dc1394video_mode_t mode;
    dc1394framerate_t rate;
    Camera::Configuration c;
    Camera::Configuration current;
    bool currentSet = false;

    // Query available modes
    if (dc1394_video_get_supported_modes( camera, &videoModes ) != DC1394_SUCCESS )
	throw CameraException(CameraException::CONNECTIONISSUE);

    // Query the current mode
    if( dc1394_video_get_mode(this->camera, &mode ) != DC1394_SUCCESS )
	throw CameraException(CameraException::CONNECTIONISSUE);

    // Query the current framerate
    if( dc1394_video_get_framerate(this->camera, &rate ) != DC1394_SUCCESS )
	throw CameraException(CameraException::CONNECTIONISSUE);

    // TODO: Note we don't currently support format 7 - benjsc 20110318

    // Walk through all supported modes and frame rates of the camera, whilst
    // doing this we also check for the current mode the camera is in
    for( unsigned i = 0; i < videoModes.num; i++ )
    {
	unsigned j;
	for( j = 0; j < ARRAY_SIZE(formatConversion); j++ ){
	    // Check if we have the current mode
	    if( formatConversion[j].dc1394mode == mode ){
		current.width = formatConversion[j].libwclwidth;
		current.height = formatConversion[j].libwclheight;
		current.format = formatConversion[j].libwclmode;
	    }

	    if( formatConversion[j].dc1394mode == videoModes.modes[i]){
		c.width = formatConversion[j].libwclwidth;
		c.height = formatConversion[j].libwclheight;
		c.format = formatConversion[j].libwclmode;
		break;
	    }

	}
	// If a mode exists that libwcl doesn't support simply process the next mode
	if ( j == ARRAY_SIZE(formatConversion))
	    continue;

	// loop through all of the framerates and add them as available
	// configurations. We also check for the current framerate here
	dc1394_video_get_supported_framerates( this->camera, videoModes.modes[i], &framerates );
	for( uint32_t i = 0; i <= framerates.num - 1; i++ ) {
	    for( j = 0 ; j < ARRAY_SIZE(fpsConversion); j++ ){

		// Check if we have the current framerate
		if( fpsConversion[j].dc1394fps == rate ){
		    current.fps = fpsConversion[j].libwclfps;
		    currentSet = true;
		}

		if( fpsConversion[j].dc1394fps == framerates.framerates[i]){
		    c.fps = fpsConversion[j].libwclfps;
		    this->supportedConfigurations.push_back(c);
		}
	    }
	}
    }

    // During the loops we should have set the current mode/fps
    // if we have store it here
    if( currentSet == false )
	throw CameraException(CameraException::INVALIDFORMAT);

    Camera::setConfiguration(current);
}

void DC1394Camera::printDetails(bool state)
{
    Camera::printDetails(state);

    if ( state ) {
	dc1394featureset_t features;

	// grab the set of features from the camera.
	dc1394_feature_get_all( this->camera, &features );

	// display the features
	dc1394_feature_print_all(&features, stderr );
    }
}


}
