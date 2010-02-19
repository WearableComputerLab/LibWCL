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

#ifndef WCL_CAMERA_CAMERAFACTORY_H
#define WCL_CAMERA_CAMERAFACTORY_H

#include <vector>
#include <wcl/camera/Camera.h>

#warning CameraFactory: Note the *CameraFactoryAPI/Implementation is currently Experimental -benjsc 20100212

namespace wcl
{
	/**
	 * The Camera Factory singleton class provides a simple method to obtain bus independant cameras
	 * from the system. Cameras are represented by a CameraID which is unique to the camera.
	 * Each of these methods return NULL if there is no camera matching the
	 * requirements of the method call
	 */
	class CameraFactory
	{
		public:

			/**
			 * Obtain any camera
			 */
			static Camera *getCamera();

			/**
			 * Obtain the camera with the specified ID
			 */
			static Camera *getCamera(const Camera::CameraID);

			/**
			 * Obtain a list of all usable cameras
			 */
			static std::vector<Camera *>getCameras();

			/**
			 * Find a camera with the specified image format,
			 * optionally restricting the matched camera by widht,
			 * height or fps if that parameter is not zero
			 */
			static Camera *findCamera(const Camera::ImageFormat f,
						  const unsigned width=0, const
						  unsigned height=0,
						  const unsigned fps=0);

			/**
			 * Find all cameras matching the specified critieria
			 */
			static std::vector<Camera *> findCameras(const Camera::ImageFormat f,
								 const unsigned width=0,
								 const unsigned height=0,
								 const unsigned fps=-1);
		private:
			CameraFactory();
			static CameraFactory *getInstance();

			static CameraFactory *instance;
	};
};

#endif

