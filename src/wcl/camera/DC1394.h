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
#ifndef WCL_CAMERA_DC1394_H
#define WCL_CAMERA_DC1394_H

#include <wcl/camera/Camera.h>

namespace wcl {

#include <dc1394/control.h>
#include <dc1394/conversions.h>

class DC1394: public Camera
{
public:
	// default constructor
	DC1394();

	// deconstructor
	~DC1394();

	void startup();

	// method to get a frame from the camera
	const unsigned char* getFrame();

	// method to shut down the camera.
	void shutdown();

	void printDetails();

	void setFormat( const ImageFormat f, const unsigned width, const
			unsigned height );


	// method to set exposure
	bool setExposureMode( const ExposureMode t );

	/**
	 * Sets the control value for the camera
	 */
	bool setControlValue(const Control control, const int value);

private:
	void close_camera();

	//XXX NOTE THE Below should be adapted to the wcl/camera/Camera.h API
	//XXX Or the api updated!

	// method to change to the video mode specified by the string.
	void setVideoMode( char* newVideoMode );

	// method to change to the framerate specified by the string.
	void setFramerate( char* newFramerate );

	// method to change to the ISO speed
	void setISOSpeed( char* newISOSpeed );

	void setShutter( char* shutter );
	// method to set brightness
	void setBrightness( char* brightness );

	// method to set the gain
	void setGain( char* gain );

	// tell the camera that it is to work in trigger mode
	void useTrigger( bool turnOn );

	// returns the width of the image we are working with
	int getWidth();

	// returns the height of the image we are working with
	int getHeight();

	// set the flag that tells us if we need to convert from a bayer image or not
	void setUseBayer( bool useBayer );

	// return whether or not we need to convert from bayer or not
	bool getUseBayer();

	// find out what sort of conversion method to use.
	void setBayerMethod( char* bayerMethod );

	// find out what sort of bayer color filter to use
	void setColorFilter( char* colorFilter );

	void cleanup( void );

	// method to populate videoModes with data
	void getSupportedVideoModes();

	// method to populate framerates with data
	void getSupportedFramerates();

	// method to check to see if a video mode is supported
	bool isVideoModeSupported( dc1394video_mode_t videoMode );

	// method to check to see if a given framerate is supported
	bool isFramerateSupported( dc1394framerate_t framerate );

	// configuration method that reads in values from a config file.
	//void configure();

	// fetch the image dimensions based on the camera setup
	void fetchImageProperties();

	// method to display to the user the different video modes available
	void printSupportedVideoModes();

	// method to display to the user the different framerates available
	void printSupportedFramerates();

	// display a bunch of info about what the camera is capable of
	void printFeatureSet();

	// the camera that we are interested in.
	dc1394camera_t* camera;

	// an array of all the cameras that are attatched to the system.
	dc1394camera_t **cameras;

	// the number of cameras that are attached to the system
	uint32_t numCameras;

	// a frame returned from the dc1394 capture device
	dc1394video_frame_t* frame;
	dc1394video_mode_t videoMode;
	dc1394video_modes_t videoModes;
	dc1394framerates_t framerates;
	dc1394framerate_t framerate;

	// some memory to hold the error code that is returned from the functions.
	dc1394error_t result;

	unsigned char* convertedImage;

	// boolean flag that says whether or not we are converting from bayer or not
	bool useBayer;

	// if we are converting from bayer then use this filter
	dc1394color_filter_t colorFilter;

	// if we are converting from bayer then use this method
	dc1394bayer_method_t bayerMethod;

	// the number of pixels wide the image is
	uint32_t width;

	// the number of pixels high the image is
	uint32_t height;

	// the depth of the image
	uint32_t depth;

	bool running;
};

};
#endif

/*
Return-Path: <aaron@staffordfarm.com.au>
Received: from reason.ml.unisa.edu.au (reason.ml.unisa.edu.au [130.220.164.145])
    by cis-mail.ml.unisa.edu.au (8.14.2/8.14.2) with ESMTP id o151r8w4035640
    for <closebs@cis-mail.ml.unisa.edu.au>; Fri, 5 Feb 2010 12:23:08 +1030 (CST)
    (envelope-from aaron@staffordfarm.com.au)
Received: from mail.clearchain.com (leo.clearchain.com [199.73.29.74])
    by reason.ml.unisa.edu.au (8.12.10/8.12.10) with ESMTP id o151r6Mf021906
    for <cisbjc@cs.unisa.edu.au>; Fri, 5 Feb 2010 12:23:07 +1030 (CST)
Received: from [192.168.1.5] (ppp118-210-204-239.lns20.adl6.internode.on.net [118.210.204.239])
    (authenticated bits=0)
    by mail.clearchain.com (8.14.3/8.14.3) with ESMTP id o151TaOx090871
    (version=TLSv1/SSLv3 cipher=DHE-RSA-CAMELLIA256-SHA bits=256 verify=NO)
    for <cisbjc@cs.unisa.edu.au>; Fri, 5 Feb 2010 11:59:45 +1030 (CST)
    (envelope-from aaron@staffordfarm.com.au)
Subject: Re: DC1394 Code License in LibWCL
From: Aaron Stafford <aaron@staffordfarm.com.au>
To: Benjamin Close <cisbjc@cs.unisa.edu.au>
In-Reply-To: <4B6B63E0.2080505@cs.unisa.edu.au>
References: <4B6B63E0.2080505@cs.unisa.edu.au>
Content-Type: text/plain
Date: Fri, 05 Feb 2010 11:59:35 +1030
Message-Id: <1265333375.4665.2.camel@aaron-desktop>
Mime-Version: 1.0
X-Mailer: Evolution 2.26.1 
Content-Transfer-Encoding: 7bit
X-Greylist: Sender IP whitelisted, not delayed by milter-greylist-4.0 (cis-mail.ml.unisa.edu.au [130.220.236.63]); Fri, 05 Feb 2010 12:23:08 +1030 (CST)
X-Greylist: Delayed for 00:23:14 by milter-greylist-2.0.2 (reason.ml.unisa.edu.au [130.220.164.145]); Fri, 05 Feb 2010 12:23:07 +1030 (CST)
X-Greylist: Sender succeeded SMTP AUTH, not delayed by milter-greylist-4.0.1 (mail.clearchain.com [199.73.29.74]); Fri, 05 Feb 2010 11:59:46 +1030 (CST)
X-Virus-Scanned: ClamAV version 0.94, clamav-milter version 0.94 on cis-mail.ml.unisa.edu.au
X-Virus-Status: Clean
X-DSPAM-Result: Whitelisted
X-DSPAM-Processed: Fri Feb  5 12:23:08 2010
X-DSPAM-Confidence: 0.9793
X-DSPAM-Probability: 0.0000
X-DSPAM-Signature: 4,4b6b7a04356425209328925

Hi Ben, 

BSD license is fine.

Aaron.

On Fri, 2010-02-05 at 10:48 +1030, Benjamin Close wrote:
> Hi Stafford,
>      I'm currently updating the camera support in libwcl. However your 
> DC1394 code has no license listed.
> Are you happy for it to be under BSD license? (Since you created it :)
> 
> Cheers,
>      Benjamin
> 

*/
