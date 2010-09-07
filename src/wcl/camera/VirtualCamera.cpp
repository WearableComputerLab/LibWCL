/*-
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

#include <iostream>
#include "VirtualCamera.h"

#define guint int
#define guint8 const char *
#include "VirtualCameraDefaultImage.c"

using namespace std;

namespace wcl
{

Camera::CameraBuffer VirtualCamera::defaultBuffer;

VirtualCamera::VirtualCamera()
{
    if(VirtualCamera::defaultBuffer.start==NULL){
	VirtualCamera::defaultBuffer.start=(void *)gimp_image.pixel_data[0]; // NOTE: CONST LOST
	VirtualCamera::defaultBuffer.length=sizeof(gimp_image.pixel_data)/sizeof(gimp_image.pixel_data[0]);
    }

	Configuration c;
	c.format=Camera::RGB8;
	c.width=640;
	c.height=480;
	c.fps=1;
	supportedConfigurations.push_back(c);
    setConfiguration(c);
}

VirtualCamera::~VirtualCamera()
{}

void VirtualCamera::printDetails(bool state)
{
    Camera::printDetails(state);

    if ( state ){
	if ( this->buffers ){
	    cout << "Using Provided Frames" << endl;
	    cout << "| Current Frame: " << inUseBuffer << endl;
	    cout << "| Total Frames: " << this->numBuffers << endl;
	} else {

	    cout << "Using Default Frame" << endl;
	}
    }
}

void VirtualCamera::setConfiguration(Configuration c)
{
	if(c.format = Camera::RGB8)
		Camera::setConfiguration(c);
	else
		cout << "VirtualCamera:SetFormat: Virtual Camera default image only supports RGB" << endl;
}

void VirtualCamera::setExposureMode(const ExposureMode t)
{
    cout << "VirtualCamera: Confirming Exposure Mode change "
	 << "(Note: No change to the image will occur)" << endl;
}

void VirtualCamera::setControlValue(const Control control, const int value)
{
    cout << "VirtualCamera: Confirming control " << control << "Set to value "
	 << value << "(Note: No change to the image will occur)" << endl;
}

void VirtualCamera::update()
{
	if( this->buffers ){
		unsigned char *frame =
			(unsigned char *) this->buffers[this->inUseBuffer].start;
		this->inUseBuffer++;
		if( this->inUseBuffer > this->numBuffers){
			this->inUseBuffer = 0;
		}
		currentFrame = frame;
		return;
	}
	currentFrame = (unsigned char*) defaultBuffer.start;
}

void VirtualCamera::startup()
{}

void VirtualCamera::shutdown()
{
    this->buffers=NULL;
    this->inUseBuffer=0;
    this->numBuffers=0;
}


void VirtualCamera::setFrames(const CameraBuffer *buffers, const unsigned bufferCount)
{
    this->buffers=const_cast<CameraBuffer *>(buffers); //NOTE: cost removed, though the buffer is still treated as read only internally
    this->numBuffers = bufferCount;
    this->inUseBuffer=0;
}


}; //namespace wcl
