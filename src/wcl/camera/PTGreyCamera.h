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
#ifndef WCL_CAMERA_PTGREYCAMERA_H
#define WCL_CAMERA_PTGREYCAMERA_H

#include <flycapture/FlyCapture2.h>
#include <wcl/api.h>
#include <wcl/camera/Camera.h>

namespace wcl {

/**
 * A Class representing a camera using Point Grey's
 * FlyCapture2 library. At present it only supports
 * GiGE cameras
 */
class WCL_API PTGreyCamera: public Camera
{
public:
	// default constructor
	PTGreyCamera(const FlyCapture2::PGRGuid guid);

	// deconstructor
	~PTGreyCamera();

	virtual void startup();
	virtual void shutdown();
	virtual void update();

	void printDetails(bool);


	void setExposureMode(ExposureMode) { /*FIXME*/ }

	void setControlValue(const Control control, const int value);
	int  getControlValue(const Control control);
	void setConfiguration(const Configuration &c);
	const char *getTypeIdentifier() const { return "PTGrey"; };

private:
	FlyCapture2::PGRGuid ptid;
	FlyCapture2::Image rawImage;
	FlyCapture2::Camera camera;
	FlyCapture2::Format7Info cameraInfo;
	void probeCamera();
};

};
#endif
