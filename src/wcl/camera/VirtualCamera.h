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

#ifndef WCL_CAMERA_VIRTUALCAMERA_H
#define WCL_CAMERA_VIRTUALCAMERA_H

#include <wcl/api.h>
#include <wcl/camera/Camera.h>

namespace wcl
{

	/**
	 * The virtual camera allows a single frame or list of frames to be used
	 * to represent a camera. All frames provided to the class are
	 * maintained untouched. If no frames are provided, a default frame is
	 * show. Upon a shutdown, the default frame is used again.
	 */
	class WCL_API VirtualCamera: public Camera
	{

	   public:
		    VirtualCamera();
		    ~VirtualCamera();

		    // Overrides of Camera
		    virtual void printDetails(bool);
		    virtual void setConfiguration(Configuration c);
		    virtual void setExposureMode(const ExposureMode t);
		    virtual void setControlValue(const Control control, const int value);
		    virtual const unsigned char* getFrame();
		    virtual void startup();
		    virtual void shutdown();

		    /**
		     * Set the data used by the virtual camera.
		     * The Virtual Camera doesn't know how to load images
		     * however it knows about buffer data and hence makes use of
		     * the buffers to provide frames
		     */
		    void setFrames(const CameraBuffer *buffers, const unsigned bufferCount);

	    protected:
		    const char *getTypeIdentifier() const { return "VIRTUAL"; }

	    private:
		    static CameraBuffer defaultBuffer;
		    unsigned inUseBuffer;
	};

};

#endif
