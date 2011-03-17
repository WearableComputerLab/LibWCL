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

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define DEBUG(x) x

namespace wcl {

struct ptGreylibWCL
{
    PixelFormat ptGrey;
    Camera::ImageFormat libwcl;
};

static ptGreylibWCL formatConversion[10] =
{
	{ PIXEL_FORMAT_MONO8, Camera::MONO8 },
	{ PIXEL_FORMAT_RGB8, Camera::RGB8 },
	{ PIXEL_FORMAT_MONO16, Camera::MONO16 },
	{ PIXEL_FORMAT_RGB16, Camera::RGB16 },
	{ PIXEL_FORMAT_411YUV8, Camera::YUYV411 },
	{ PIXEL_FORMAT_422YUV8, Camera::YUYV422 },
	{ PIXEL_FORMAT_444YUV8, Camera::YUYV444 },
	{ PIXEL_FORMAT_RAW8, Camera::RAW8 },
	{ PIXEL_FORMAT_RAW16, Camera::RAW16 },
	{ PIXEL_FORMAT_BGR, Camera::BGR8 }

       /* The following modes are not implemented in libwcl - benjsc 20110310
	PIXEL_FORMAT_S_MONO16
	PIXEL_FORMAT_S_RGB16
	PIXEL_FORMAT_MONO12
	PIXEL_FORMAT_RAW12
	PIXEL_FORMAT_BGRU
	PIXEL_FORMAT_RGBU
	*/
};

PTGreyCamera::PTGreyCamera(const PGRGuid iguid):
    ptid(iguid),capturing(false)
{
    Error error;

    stringstream stream;
    stream << iguid.value[0];
    stream << iguid.value[1];
    stream << iguid.value[2];
    stream << iguid.value[3];
    this->id = stream.str();
    this->probeCamera();
}

PTGreyCamera::~PTGreyCamera()
{
    this->shutdown();
}

void PTGreyCamera::startup()
{
    Error error;

    if( !this->camera.IsConnected())
	this->connect();

    if( !this->capturing ){
	error=this->camera.StartCapture();
	if(error != PGRERROR_OK){
	    DEBUG(error.PrintErrorTrace());
	    throw CameraException(CameraException::CONNECTIONISSUE);
	}
	this->capturing = true;
    }
}

void PTGreyCamera::shutdown()
{
    if( this->capturing ){
	this->camera.StopCapture();
	this->capturing = false;
    }

    if( this->camera.IsConnected())
	this->camera.Disconnect();
}

void PTGreyCamera::connect()
{
    Error error;

    error=this->camera.Connect(&this->ptid);
    if(error != PGRERROR_OK){
	DEBUG(error.PrintErrorTrace());
	throw CameraException(CameraException::CONNECTIONISSUE);
    }
}

void PTGreyCamera::update()
{
    Error error;
    if( !this->capturing)
	this->startup();

    error = this->camera.RetrieveBuffer( &this->rawImage );
    if( error != PGRERROR_OK ){
	DEBUG(error.PrintErrorTrace());
	throw CameraException(CameraException::BUFFERERROR);
    }

    this->currentFrame = this->rawImage.GetData();
}

void PTGreyCamera::printDetails(bool full)
{
    Camera::printDetails(full);

    if( full )
    {
	if(!this->camera.IsConnected())
	    this->connect();

	CameraInfo info;
	Error error;

	error = this->camera.GetCameraInfo(&info);
	if( error != PGRERROR_OK)
	    throw CameraException(CameraException::CONNECTIONISSUE);

	char macAddress[64];
	sprintf( macAddress,
	    "%02X:%02X:%02X:%02X:%02X:%02X",
	    info.macAddress.octets[0],
	    info.macAddress.octets[1],
	    info.macAddress.octets[2],
	    info.macAddress.octets[3],
	    info.macAddress.octets[4],
	    info.macAddress.octets[5]);

	char ipAddress[32];
	sprintf( ipAddress,
	    "%u.%u.%u.%u",
	    info.ipAddress.octets[0],
	    info.ipAddress.octets[1],
	    info.ipAddress.octets[2],
	    info.ipAddress.octets[3]);

	char subnetMask[32];
	sprintf( subnetMask,
	    "%u.%u.%u.%u",
	    info.subnetMask.octets[0],
	    info.subnetMask.octets[1],
	    info.subnetMask.octets[2],
	    info.subnetMask.octets[3]);

	char defaultGateway[32];
	sprintf( defaultGateway,
	    "%u.%u.%u.%u",
	    info.defaultGateway.octets[0],
	    info.defaultGateway.octets[1],
	    info.defaultGateway.octets[2],
	    info.defaultGateway.octets[3]);

	wclclog << "Model: " << info.modelName << endl
		<< "Vendor: " << info.vendorName << endl
		<< "Sensor: " << info.sensorInfo << endl
		<< "Resolution: " << info.sensorResolution << endl
		<< "MAC address: " << macAddress << endl
		<< "IP address: " << ipAddress << endl
		<< "Submnet mask: " << subnetMask << endl
		<< "Gateway: " << defaultGateway << endl
		<< "Firmware Version: " << info.firmwareVersion << endl
		<< "GigE Version: " << info.gigEMajorVersion << "." << info.gigEMinorVersion << endl;
    }
}

void PTGreyCamera::setControlValue(const Control control, const int value)
{
    switch(control)
    {
	default:
	    throw CameraException(CameraException::CONTROLERROR);
    }
}

int PTGreyCamera::getControlValue(const Control control)
{
    switch(control)
    {
	default:
	    throw CameraException(CameraException::CONTROLERROR);
    }
}

void PTGreyCamera::setConfiguration(const Configuration &c)
{
    bool valid;
    Error error;
    Format7PacketInfo fmt7PacketInfo;
    Format7ImageSettings imageSettings;
    Camera::ImageFormat which;

    // Set the format we want first
    // Find out the actual format requested
    if( c.format == Camera::FORMAT7){
	which = c.format7.format;
    } else {
	which = c.format;
    }

    unsigned i;
    for(i = 0; i < ARRAY_SIZE( formatConversion ); i++ ){
	if( formatConversion[i].libwcl == which ){
	    imageSettings.pixelFormat = formatConversion[i].ptGrey;
	    break;
	}
    }
    if( i == ARRAY_SIZE(formatConversion))
	throw CameraException(CameraException::INVALIDFORMAT);

    // Now the format is set setup the camera's viewports
    // If we are emulating the camera mode we use the full viewport
    // and software scale the image
    if( c.format == Camera::FORMAT7){
	imageSettings.offsetX = c.format7.xOffset;
	imageSettings.offsetY = c.format7.yOffset;
	imageSettings.width = c.format7.xMax;
	imageSettings.height = c.format7.yMax;
    } else {

	// Emulation mode
	imageSettings.offsetX = 0;
	imageSettings.offsetY = 0;
	imageSettings.width = this->cameraInfo.maxWidth;
	imageSettings.height = this->cameraInfo.maxHeight;
    }

    if( !this->camera.IsConnected())
	this->connect();

    error = this->camera.ValidateFormat7Settings( &imageSettings, &valid, &fmt7PacketInfo );
    if (error != PGRERROR_OK || !valid) {
	throw CameraException(CameraException::INVALIDFORMAT);
    }

    this->camera.SetFormat7Configuration(&imageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
    if( error != PGRERROR_OK )
	throw CameraException(CameraException::INVALIDFORMAT);

    Camera::setConfiguration(c);
}

void PTGreyCamera::probeCamera()
{
    Error error;
    error=this->camera.Connect(&this->ptid);
    if( error != PGRERROR_OK )
	throw CameraException(CameraException::CONNECTIONISSUE);

    CameraInfo camInfo;
    error=this->camera.GetCameraInfo(&camInfo);
    if( error != PGRERROR_OK )
	throw CameraException(CameraException::CONNECTIONISSUE);

#if 0
    // Format7 consists of defined modes for pixel binning. For now we don't
    // care about these. Hence we simply keep this here for possible future
    // reference - benjsc 20100316

    for( int i = 0; i < NUM_MODES; i++ ){
	bool supported;
	Mode aMode = (Mode)i;
	error = this->camera->QueryGigEImagingMode(aMode, &supported);
	if( error != PGRERROR_OK){
	    continue;
	}
    }
#endif

    // The PTGrey GigECameras are all format 7. We emulate the
    // other modes supported by libwcl Camera by displaying the width and the
    // height of each of these. 

    bool supported;
    error=this->camera.GetFormat7Info(&this->cameraInfo,&supported);
    if( error != PGRERROR_OK )
	throw CameraException(CameraException::CONNECTIONISSUE);

    // We now have the format 7 mode information hence we can use this
    // to determine supported configurations
#if 0
    wclclog << "maxWidth: " << info.maxWidth << endl;
    wclclog << "maxHeight: " << info.maxHeight << endl;
    wclclog << "imageHStep: " << info.imageHStepSize << endl;
    wclclog << "imageVStep: " << info.imageVStepSize << endl;
    wclclog << "bitfield: " << info.pixelFormatBitField << endl;
#endif
    Configuration c;
    c.format = FORMAT7;
    for(unsigned i = 0; i < ARRAY_SIZE( formatConversion ); i++ ){
	c.format7.xOffset = 0;
	c.format7.yOffset = 0;
	c.format7.xOffsetStepSize = this->cameraInfo.offsetHStepSize;
	c.format7.yOffsetStepSize = this->cameraInfo.offsetVStepSize;
	c.format7.xMax = this->cameraInfo.maxWidth;
	c.format7.yMax = this->cameraInfo.maxHeight;
	c.format7.xStepSize = this->cameraInfo.imageHStepSize;
	c.format7.yStepSize = this->cameraInfo.imageVStepSize;
	c.format7.format = formatConversion[i].libwcl;
	this->supportedConfigurations.push_back(c);
    }

    // Create the emulated modes
    // These use common sizes to build modes
    struct Sizes {
	unsigned width;
	unsigned height;
    } commonSizes[] = {
	{320,200},
	{640,480},
	{800,600},
	{1024,768},
	{1280,720},
	{1280,800},
	{1280,1024}
    };

    c.fps = -1;
    for(unsigned i= 0; i < ARRAY_SIZE(commonSizes) + 1; i++ ){
	if( i == ARRAY_SIZE(commonSizes)){
	    // Add the maximum resolution if not already
	    // recored
	    if( this->cameraInfo.maxWidth != commonSizes[i-1].width &&
		this->cameraInfo.maxHeight != commonSizes[i-1].height ){
		c.width = this->cameraInfo.maxWidth;
		c.height = this->cameraInfo.maxHeight;
	    }
	} else {
	    // Add the preset resolutions if supported
	    // by the camera
	    Sizes s = commonSizes[i];
	    if( s.width < this->cameraInfo.maxWidth &&
		s.width % this->cameraInfo.imageHStepSize == 0 &&
		s.height < this->cameraInfo.maxHeight &&
		s.height % this->cameraInfo.imageVStepSize == 0 ){
		c.width = s.width;
		c.height = s.height;
	    }
	    else {
		continue;
	    }
	}
	for(unsigned i = 0; i < ARRAY_SIZE( formatConversion ); i++ ){
	    if(this->cameraInfo.pixelFormatBitField & formatConversion[i].ptGrey){
		c.format = formatConversion[i].libwcl;
		this->supportedConfigurations.push_back(c);
	    }
	}
    }

    //
    // Set the current resolution
    //
    unsigned packetSize;
    float percent;
    Format7ImageSettings imageSettings;
    error=this->camera.GetFormat7Configuration(&imageSettings,&packetSize,&percent );
    if( error != PGRERROR_OK )
	throw CameraException(CameraException::CONNECTIONISSUE);

    // Add the max resolution
    c.format7.xOffset = imageSettings.offsetX;
    c.format7.yOffset = imageSettings.offsetY;
    c.format7.xMax = imageSettings.width;
    c.format7.yMax = imageSettings.height;
    c.format7.xStepSize = 0;
    c.format7.yStepSize = 0;
    c.format = FORMAT7;

    unsigned i;
    for(i = 0; i < ARRAY_SIZE( formatConversion ); i++ ){
	if( imageSettings.pixelFormat == formatConversion[i].ptGrey ){
	    c.format7.format = formatConversion[i].libwcl;
	    break;
	}
    }
    if( i == ARRAY_SIZE(formatConversion))
	throw CameraException(CameraException::INVALIDFORMAT);

    Camera::setConfiguration(c);

    this->camera.Disconnect();
}


} //namespace wcl
