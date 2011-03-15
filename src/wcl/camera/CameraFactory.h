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
#include <wcl/api.h>
#include <wcl/camera/Camera.h>

namespace wcl
{
	/**
	 * The Camera Factory singleton class provides a simple method to obtain bus independant cameras
	 * from the system. Cameras are represented by a CameraID which is unique to the camera.
	 * Each of these methods return NULL if there is no camera matching the
	 * requirements of the method call
	 */
	class WCL_API CameraFactory
	{
		public:
			enum SearchScope { LOCAL, NETWORK, ALL };

			/**
			 * Obtain any camera, by default this will obtain the
			 * first available camera locally. Specifying NETWORK or
			 * ALL will return the first available local camera if
			 * available or the first available network camera if
			 * available. If no cameras are available an exception
			 * is thrown
			 *
			 * @param SearchScope The scope to search for a camera
			 * @throw CameraException if no camera of that search scope is found
			 * @return A valid camera object
			 */
			static Camera *getCamera(const SearchScope = LOCAL);

			/**
			 * Obtain the camera with the specified ID. The camera
			 * is search across all available SearchMedia unless
			 * restricted
			 *
			 * @param CameraID The id of the camera to find
			 * @param SearchScope The scope to search for a camera
			 * @throw CameraException if a camera with the given ID cannot be found
			 * @return A valid camera object
			 */
			static Camera *getCamera(const Camera::CameraID, const SearchScope = ALL);

			/**
			 * Obtain a list of all usable cameras based on the
			 * given search Scope
			 *
			 * @param SearchScope The scope to search for a camera
			 * @return A vector of available cameras, if no camera is avaliable the vector is empty
			 */
			static std::vector<Camera *>getCameras(const SearchScope = LOCAL);

			/**
			 * Find a camera with the specified image format,
			 * optionally restricting the matched camera by widht,
			 * height or fps if that parameter is not zero
			 *
			 * @param partialConfig A partially/complete filled in configuration for the camera
			 * @param SearchScope The scope to search for a camera
			 */
			static Camera *findCamera(Camera::Configuration partialConfig, const SearchScope=LOCAL);

			/**
			 * Find all cameras matching the specified critieria.
			 * The searching scope is restricted to local cameras by default
			 *
			 * @param partialConfig A partially/complete filled in configuration for the camera
			 * @param SearchScope The scope to search for a camera
			 */
			static std::vector<Camera *> findCameras(Camera::Configuration partialConfig, const SearchScope=LOCAL);

			/**
			 * Display information about all cameras in the system
			 *
			 * @param full If true display full details about each camera, else short info about the cameras
			 * @param SearchScope The scope to print details about the cameras
			 */
			static void printDetails(bool full, const SearchScope=ALL);

		private:
			CameraFactory();
			static CameraFactory *getInstance();

			static CameraFactory *instance;
	};
};

#endif

