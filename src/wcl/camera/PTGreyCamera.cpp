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
#include <stdlib.h>
#include <sstream>
#include "wcl/IO.h"
#include "wcl/camera/CameraException.h"
#include "wcl/camera/PTGreyCamera.h"

using namespace std;
using namespace FlyCapture2;

namespace wcl {

PTGreyCamera::PTGreyCamera(const PGRGuid iguid):
    ptid(iguid)
{
    Error error;

    stringstream stream;
    stream << iguid.value[0];
    stream << iguid.value[1];
    stream << iguid.value[2];
    stream << iguid.value[3];
    this->id = stream.str();
    this->probeCamera();

    this->setConfiguration(this->supportedConfigurations[0]);

    this->startup();

}

PTGreyCamera::~PTGreyCamera()
{
    this->shutdown();
}

void PTGreyCamera::startup()
{
    Error error;
    /*
    this->camera.Connect(&this->ptid);
    if( error != PGRERROR_OK )
	throw CameraException("CAMERA PROBE ERROR");
	*/

    error=this->camera.StartCapture();
    if(error != PGRERROR_OK){
	error.PrintErrorTrace();
	throw CameraException("CAPTURE ERROR1");
    }
}

void PTGreyCamera::shutdown()
{
    this->camera.StopCapture();
    this->camera.Disconnect();
}

void PTGreyCamera::update()
{
    Error error;
    error = this->camera.RetrieveBuffer( &this->rawImage );
    if( error != PGRERROR_OK ){
	error.PrintErrorTrace();
	throw CameraException("CAPTURE ERROR2");
    }

    this->currentFrame = this->rawImage.GetData();
}

void PTGreyCamera::printDetails(bool full)
{
    Camera::printDetails(full);

    if( full )
    {
	//TODO: Print IP Address Details, etc - benjsc 20101220
    }
}

void PTGreyCamera::setControlValue(const Control control, const int value)
{}

void PTGreyCamera::setConfiguration(const Configuration &c)
{
    wclcerr << "PTGreyCamera::setConfigureation:: NOT IMPLEMENTED" << endl;
    //exit(1);
    this->activeConfiguration = c;
}

void PTGreyCamera::probeCamera()
{
    Error error;
    error=this->camera.Connect(&this->ptid);
    if( error != PGRERROR_OK )
	throw CameraException("CAMERA PROBE ERROR1");

    CameraInfo camInfo;
    error=this->camera.GetCameraInfo(&camInfo);
    if( error != PGRERROR_OK )
	throw CameraException("CAMERA PROBE ERROR2");

    /*
    for( int i = 0; i < NUM_MODES; i++ ){
	bool supported;
	Mode aMode = (Mode)i;
	error = this->camera->QueryGigEImagingMode(aMode, &supported);
	if( error != PGRERROR_OK){
	    continue;
	}
    }

    // Get supported pixelformts
    GigEImageSettingsInfo info;
    error=this->camera->getGigEImageSettingsInfo(&info);
    if( error != PGRERROR_OK )
	throw CameraException("CAMERA PROBE ERROR");
    */


    GigEImageSettings imageSettings;
    error=this->camera.GetGigEImageSettings(&imageSettings);
    if( error != PGRERROR_OK )
	throw CameraException("CAMERA PROBE ERROR3");

    // Fake the following modes
    Configuration c;
    c.format = this->ptGreyPixelFormatTolibWCLImageFormat(imageSettings.pixelFormat);
    c.width = imageSettings.width;
    c.height = imageSettings.height;
    c.fps = 0;
    this->supportedConfigurations.push_back(c);

    this->camera.SetGigEImageSettings(&imageSettings);
    if( error != PGRERROR_OK )
	throw CameraException("CAMERA PROBE ERROR4");

//    this->camera.Disconnect();
}

Camera::ImageFormat PTGreyCamera::ptGreyPixelFormatTolibWCLImageFormat(const PixelFormat p)
{
    switch(p)
    {
	case PIXEL_FORMAT_MONO8: return MONO8;
	case PIXEL_FORMAT_RGB8: return RGB8;
	case PIXEL_FORMAT_MONO16: return MONO16;
	case PIXEL_FORMAT_RGB16: return RGB16;
	case PIXEL_FORMAT_S_MONO16:
	case PIXEL_FORMAT_S_RGB16:
	case PIXEL_FORMAT_RAW8:
	case PIXEL_FORMAT_RAW16:
	case PIXEL_FORMAT_MONO12:
	case PIXEL_FORMAT_RAW12:
	case PIXEL_FORMAT_BGR:
	case PIXEL_FORMAT_BGRU:
	case PIXEL_FORMAT_RGBU:
	case PIXEL_FORMAT_411YUV8:
	case PIXEL_FORMAT_422YUV8:
	case PIXEL_FORMAT_444YUV8:
				 throw CameraException("Unknown Format");
    }
}

} //namespace wcl
