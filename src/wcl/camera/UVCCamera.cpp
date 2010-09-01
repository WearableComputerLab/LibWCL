/*-
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
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

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include "UVCCamera.h"
#include "CameraException.h"

using namespace wcl;

UVCCamera::UVCCamera(string filename) :
    isReadyForCapture(false)
{
	// Camera to open
	cam = open(filename.c_str(), O_RDWR);

	// Bail if we can't open the device...
	if (cam == -1)
	{
	    switch(errno)
	    {
		case EACCES: throw CameraException(CameraException::EACCESS);
		case ENOENT: throw CameraException(CameraException::NOTFOUND);
		default: throw CameraException(CameraException::UNKNOWN);
	    }
	}

	loadCapabilities();

	setConfiguration(supportedConfigurations[0]);

	// For now the id of a UVCCamera is it's device node
	this->id = filename;
}

UVCCamera::~UVCCamera()
{
    this->shutdown();
}


void UVCCamera::loadCapabilities()
{
	v4l2_capability info;
	ioctl(cam, VIDIOC_QUERYCAP, &info);

	/*
	 * Find out what the capabilities actually are.
	 * capabilities is an int that can be bitmasked agains capability flags.
	 * For now we are interested in VIDEO_CAPTRE and AUDIO
	 */
	if (info.capabilities & V4L2_CAP_VIDEO_CAPTURE != 1)
	{
		throw CameraException(CameraException::NOCAPTURE);
	}
	
	// mmap streaming check
	if (info.capabilities & V4L2_CAP_STREAMING)
		mode = MMAP;
	else
		mode = CALL_READ;


	//query image formats...
	v4l2_fmtdesc format;
	format.index = 0;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	//enumerate image formats
	while (0 == ioctl(cam, VIDIOC_ENUM_FMT, &format))
	{

		ImageFormat f;

		switch(format.pixelformat)
		{
			case V4L2_PIX_FMT_MJPEG:
				f=MJPEG;
				break;
			case V4L2_PIX_FMT_YUYV:
				f=YUYV422;
				break;
			case V4L2_PIX_FMT_RGB24:
				f=RGB8;
				break;
			case V4L2_PIX_FMT_BGR24:
				f=BGR8;
				break;
			case V4L2_PIX_FMT_GREY:
				f=MONO8;
				break;
			case V4L2_PIX_FMT_Y16:
				f=MONO16;
				break;
			case V4L2_PIX_FMT_JPEG:
			case V4L2_PIX_FMT_YVYU:
			case V4L2_PIX_FMT_UYVY:
			case V4L2_PIX_FMT_YYUV:
			case V4L2_PIX_FMT_YUV420:
			case V4L2_PIX_FMT_YVU420:
			case V4L2_PIX_FMT_NV12:
			case V4L2_PIX_FMT_NV21:
			case V4L2_PIX_FMT_NV16:
			case V4L2_PIX_FMT_NV61:
			case V4L2_PIX_FMT_SPCA501:
			case V4L2_PIX_FMT_SPCA505:
			case V4L2_PIX_FMT_SPCA508:
			case V4L2_PIX_FMT_SGBRG8:
			case V4L2_PIX_FMT_SGRBG8:
			case V4L2_PIX_FMT_SBGGR8:
			// Vendor Specific Extentions
			case V4L2_PIX_FMT_PWC1:
			case V4L2_PIX_FMT_PWC2:
			default:
				cerr << "UVCCamera: Ignoring unsupported video format ("  << format.pixelformat << ")" << endl;
				break;
		}

		v4l2_frmsizeenum size;
		size.index = 0;
		size.pixel_format = format.pixelformat;

		//enumerate image sizes for the image format
		while (0 == ioctl(cam, VIDIOC_ENUM_FRAMESIZES, &size))
		{
			int width = size.discrete.width;
			int height = size.discrete.height;

			//enumerate framerates at current format and resolution
			v4l2_frmivalenum frame;
			frame.index = 0;
			frame.pixel_format = format.pixelformat;
			frame.width = width;
			frame.height = height;

			while (0 == ioctl(cam, VIDIOC_ENUM_FRAMEINTERVALS, &frame))
			{
				Configuration c;
				c.format = f;
				c.fps = frame.discrete.denominator;
				c.width = width;
				c.height = height;
				this->supportedConfigurations.push_back(c);
				frame.index++;
			}
			size.index++;
		}

		format.index++;
	}

    // At this point we confirm that there in indeed supported configurations.
    // There is a posibility there is a camera that has no configurations hence
    if( this->supportedConfigurations.size() == 0 ){
	// Add a nop configuration
	// Note this really should never happen hence the values here
	// should be safe - benjsc 20100802
	Configuration c;
	c.format = (ImageFormat)65535; // No value
	c.fps=1;
	c.width=1;
	c.height=1;

	this->supportedConfigurations.push_back(c);
    }
}

void UVCCamera::setConfiguration(Configuration c)
{
	v4l2_format newf;
	newf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	newf.fmt.pix.width = c.width;
	newf.fmt.pix.height = c.height;

	switch (c.format)
	{
		case MJPEG:
			newf.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
			break;

		case YUYV422:
			newf.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
			break;

		case RGB8: 
			newf.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
			break;
		case BGR8:
			newf.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
			break;
		case MONO8:
			newf.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
			break;
		case MONO16:
			newf.fmt.pix.pixelformat = V4L2_PIX_FMT_Y16;
			break;
	}

	// lets just ignore interlacing for now
	// TODO: Fix interlacing
	newf.fmt.pix.field = V4L2_FIELD_ANY;

	if (-1 == ioctl(cam, VIDIOC_S_FMT, &newf))
	{
		throw CameraException(CameraException::INVALIDFORMAT);
	}

	bufferSize = newf.fmt.pix.sizeimage;

	Camera::setConfiguration(c);
}


const unsigned char* UVCCamera::getFrame()
{
	if (!isReadyForCapture)
	{
		prepareForCapture();
	}

	//grab frame...
	v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(cam, VIDIOC_DQBUF, &buf))
	{
		throw CameraException(CameraException::BUFFERERROR);
	}

	// requeue buffer
	ioctl(cam, VIDIOC_QBUF, &buf);

	return (const unsigned char*) buffers[buf.index].start;
}

void UVCCamera::startup()
{
    UVCCamera::prepareForCapture();
}

void UVCCamera::prepareForCapture()
{
	v4l2_requestbuffers reqbuf;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	//we are going to try for 20 buffers
	reqbuf.count = 2;

	if (-1 == ioctl(cam, VIDIOC_REQBUFS, &reqbuf))
	{
		throw CameraException(CameraException::BUFFERERROR);
	}

	// create an array for our buffers
	// The buffer count may be less than we asked for
	this->allocateBuffers(sizeof(CameraBuffer),  reqbuf.count);

	for (int i=0;i<reqbuf.count; i++)
	{
		struct v4l2_buffer buffer;
		buffer.type = reqbuf.type;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = i;

		if (-1 == ioctl(cam, VIDIOC_QUERYBUF, &buffer))
		{
		    throw CameraException(CameraException::BUFFERERROR);
		}
		buffers[i].length = buffer.length;
		buffers[i].start = mmap(NULL, buffer.length,
				PROT_READ | PROT_WRITE,
				MAP_SHARED,
				cam, buffer.m.offset);

		if (MAP_FAILED == buffers[i].start)
		{
		    throw CameraException(CameraException::BUFFERERROR);
		}

		//enqueue the buffer for use by the driver
		if (-1 == ioctl(cam, VIDIOC_QBUF, &buffer))
		{
		    throw CameraException(CameraException::BUFFERERROR);
		}
	}

	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// turn on streaming.
	if (-1 == ioctl(cam, VIDIOC_STREAMON, &type))
	{
	    throw CameraException(CameraException::BUFFERERROR);
	}


	isReadyForCapture = true;
}


void UVCCamera::shutdown()
{
	// cleanup the mmap'd buffers
	for (int i=0; i<numBuffers; i++)
		munmap(buffers[i].start, buffers[i].length);

	this->destroyBuffers();
	close(cam);
}

void UVCCamera::printDetails(bool state)
{
	Camera::printDetails(state);

	if ( state ){
	    v4l2_capability info;
	    ioctl(cam, VIDIOC_QUERYCAP, &info);

	    cout << "Camera: " << info.card << endl;
	    cout << "Bus: " << info.bus_info << endl;

	    loadControls();
	}
}

void UVCCamera::setExposureMode(const ExposureMode t)
{
	//the actual control we are setting
	struct v4l2_ext_control control;

	//the container to pass to v4l
	struct v4l2_ext_controls box;

	//setup the control
	control.id = AUTO_EXPOSURE_CTRL;
	control.value = t;
	
	//setup the container
	box.ctrl_class = V4L2_CTRL_CLASS_USER;
	box.count = 1;
	
	//normally this a pointer to an array of controls, but we are only setting 1
	box.controls = &control;

	if (0 != ioctl(cam, VIDIOC_S_EXT_CTRLS, &box)) {
		throw CameraException(CameraException::EXPOSUREERROR);
	}
}


void UVCCamera::setControlValue(const Control controlName, const int value)
{
	//the actual control we are setting
	struct v4l2_ext_control control;

	//the container to pass to v4l
	struct v4l2_ext_controls box;

	//setup the control
	control.id = mapControlToV4L2(controlName);
	control.value = value;
	
	//setup the container
	box.ctrl_class = V4L2_CTRL_CLASS_USER;
	box.count = 1;
	
	//normally this a pointer to an array of controls, but we are only setting 1
	box.controls = &control;

	if (0 != ioctl(cam, VIDIOC_S_EXT_CTRLS, &box))
	{
		throw CameraException(CameraException::CONTROLERROR);
	}

}


unsigned UVCCamera::getFormatBufferSize() const
{
	return bufferSize;
}

void UVCCamera::loadControls()
{
	// extended controls...
	v4l2_queryctrl ctrl;
	ctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
	while (0 == ioctl(cam, VIDIOC_QUERYCTRL, &ctrl))
	{
		if (ctrl.flags & V4L2_CTRL_FLAG_DISABLED)
			continue;

		if (ctrl.type == V4L2_CTRL_TYPE_MENU)
		{
			cout << "Menu:    " << ctrl.name << endl;
			cout << "   ID : " << ctrl.id << endl;

			v4l2_querymenu menu;
			menu.id = ctrl.id;
			cout << "   Menu Items:" << endl;
			for (menu.index = ctrl.minimum; menu.index <= ctrl.maximum; menu.index++)
			{
				if (0 == ioctl(cam, VIDIOC_QUERYMENU, &menu))
				{
					cout << "      " << menu.index << " " <<  menu.name << endl;
					
				}
			}
		}
		else 
		{
			cout << "Control: " << ctrl.name << endl;
			cout << "   Min: " << ctrl.minimum << endl;
			cout << "   Max: " << ctrl.maximum << endl;
			cout << "   ID : " << ctrl.id << endl;
		}
		ctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
	}

}


uint32_t UVCCamera::mapControlToV4L2( const Control c ) const
{
    switch( c )
    {
	case Camera::BRIGHTNESS: return V4L2_CID_BRIGHTNESS;
	case Camera::CONTRAST: return V4L2_CID_CONTRAST;
	case Camera::SATURATION:return V4L2_CID_SATURATION;
	case Camera::GAIN:return V4L2_CID_GAIN;
	case Camera::POWER_FREQUENCY:return V4L2_CID_POWER_LINE_FREQUENCY;
	case Camera::WHITE_BALANCE:return V4L2_CID_WHITE_BALANCE_TEMPERATURE;
	case Camera::SHARPNESS:return V4L2_CID_SHARPNESS;
	case Camera::EXPOSURE: return V4L2_CID_EXPOSURE;
    }
}

