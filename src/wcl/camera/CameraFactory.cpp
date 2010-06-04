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

#include <config.h>
#include <iostream>
#include <wcl/camera/CameraFactory.h>
#ifdef ENABLE_VIDEO_1394
#include <wcl/camera/DC1394CameraFactory.h>
#endif
#ifdef ENABLE_VIDEO_UVC
#include <wcl/camera/UVCCameraFactory.h>
#endif

namespace wcl
{

CameraFactory *CameraFactory::instance;

CameraFactory::CameraFactory()
{}


Camera *CameraFactory::getCamera()
{
    std::vector<Camera *> cameras = CameraFactory::getCameras();
    if( cameras.size() > 0 )
	return cameras[0];

    return NULL;
}

Camera *CameraFactory::getCamera(const Camera::CameraID id)
{
    std::vector<Camera *> cameras = CameraFactory::getCameras();
    for(std::vector<Camera *>::iterator it = cameras.begin();
	it!= cameras.end();
	++it ){
	Camera *c = *it;
	if( c->getID() == id )
	    return c;
    }

    return NULL;
}

std::vector<Camera *> CameraFactory::getCameras()
{
    std::vector<Camera *>all;

#ifdef ENABLE_VIDEO_1394
    try {

    std::vector<DC1394Camera *> dc1394 = DC1394CameraFactory::getCameras();

    for(std::vector<DC1394Camera *>::iterator it = dc1394.begin();
	it != dc1394.end();
	++it )
	all.push_back( *it );

    } catch ( std::string s ){
	std::cout << "DC1394Cameras Unavailable:" << s << std::endl;
    }
#endif

#ifdef ENABLE_VIDEO_UVC
    std::cout << "Searching for USB Cameras...\n" << endl;
    std::vector<UVCCamera *> uvc = UVCCameraFactory::getCameras();
    for(std::vector<UVCCamera *>::iterator it = uvc.begin();
	it != uvc.end();
	++it )
	all.push_back( *it );
#endif

    return all;
}


Camera *CameraFactory::findCamera(const Camera::ImageFormat f,
				  const unsigned width, const unsigned height,
				  const unsigned fps )
{
    std::vector<Camera *> cameras =  CameraFactory::findCameras(f, width, height, fps );
    if( cameras.size() > 0 ){
	return cameras[0];
    }
    return NULL;
}

std::vector<Camera *> CameraFactory::findCameras(const Camera::ImageFormat f,
				  const unsigned width, const unsigned height,
				  const unsigned fps )
{
    std::vector<Camera *> cameras = getCameras();
    std::vector<Camera *> matches;

    // FIXME Check each camera that supports this...

    for(std::vector<Camera *>::iterator it = cameras.begin();
	it!= cameras.end();
	++it ){
	Camera *c = *it;

	if ( c->getImageFormat() != f )
	    continue;

	if( width > 0 && c->getFormatWidth() != width )
	    continue;

	if( height > 0 && c->getFormatHeight() != height )
	    continue;

	if( fps > 0 && c->getFormatFPS() != fps )
	    continue;

	matches.push_back(c);

    }

    return matches;
}

CameraFactory * CameraFactory::getInstance()
{
    if( CameraFactory::instance == NULL )
	CameraFactory::instance = new CameraFactory();

    return CameraFactory::instance;
};


}; //namespace
