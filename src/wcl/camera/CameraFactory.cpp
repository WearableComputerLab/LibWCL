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
#include <wcl/IO.h>
#include <wcl/camera/CameraException.h>
#include <wcl/camera/CameraFactory.h>
#ifdef ENABLE_CAMERA_DC1394
#include <wcl/camera/DC1394CameraFactory.h>
#endif
#ifdef ENABLE_CAMERA_UVC
#include <wcl/camera/UVCCameraFactory.h>
#endif
#ifdef ENABLE_CAMERA_PTGREY
#include <wcl/camera/PTGreyCameraFactory.h>
#endif

using namespace std;

namespace wcl
{

CameraFactory *CameraFactory::instance;

CameraFactory::CameraFactory()
{}


Camera *CameraFactory::getCamera(const SearchScope scope)
{
    std::vector<Camera *> cameras = CameraFactory::getCameras(scope);
    if( cameras.size() > 0 )
	return cameras[0];

    return NULL;
}

Camera *CameraFactory::getCamera(const Camera::CameraID id, const SearchScope scope)
{
    std::vector<Camera *> cameras = CameraFactory::getCameras(scope);
    for(std::vector<Camera *>::iterator it = cameras.begin();
	it!= cameras.end();
	++it ){
	Camera *c = *it;
	if( c->getID() == id )
	    return c;
    }

    return NULL;
}

std::vector<Camera *> CameraFactory::getCameras(const SearchScope scope)
{
    std::vector<Camera *>all;

#ifdef ENABLE_CAMERA_PTGREY
    if( scope != LOCAL ){

	try {
	    std::vector<PTGreyCamera *>ptgrey = PTGreyCameraFactory::getCameras();
	    for(std::vector<PTGreyCamera *>::iterator it = ptgrey.begin();
		it != ptgrey.end();
		++it )
		all.push_back( *it );

	} catch ( CameraException &e ){
	    wclclog << "PTGreyCameras Unavailable:" << e.what() << std::endl;
	}
    }
#endif

#ifdef ENABLE_CAMERA_DC1394
    if( scope != NETWORK ){
	try {

	    std::vector<DC1394Camera *> dc1394 = DC1394CameraFactory::getCameras();

	    for(std::vector<DC1394Camera *>::iterator it = dc1394.begin();
		it != dc1394.end();
		++it )
		all.push_back( *it );

	} catch ( CameraException &e ){
	    wclclog << "DC1394Cameras Unavailable:" << e.what() << std::endl;
	}
    }
#endif

#ifdef ENABLE_CAMERA_UVC
    if ( scope != NETWORK ){
	try {
	    std::vector<UVCCamera *> uvc = UVCCameraFactory::getCameras();
	    for(std::vector<UVCCamera *>::iterator it = uvc.begin();
		it != uvc.end();
		++it )
		all.push_back( *it );
	} catch (CameraException &e){
	    wclclog << "UVC Camera(s) Unavailable:" << e.what() << std::endl;
	}
    }
#endif

    return all;
}


Camera *CameraFactory::findCamera(Camera::Configuration partialConfig, const SearchScope scope)
{
    std::vector<Camera *> cameras =  CameraFactory::findCameras(partialConfig, scope);
    if( cameras.size() > 0 ){
	return cameras[0];
    }
    return NULL;
}

std::vector<Camera *> CameraFactory::findCameras(Camera::Configuration partialConfig, const SearchScope scope)
{
    std::vector<Camera *> cameras = getCameras(scope);
    std::vector<Camera *> matches;

    for(std::vector<Camera *>::iterator it = cameras.begin();
	it!= cameras.end();
	++it ){

		Camera *c = *it;
		try
		{
			Camera::Configuration config = c->findConfiguration(partialConfig);
			c->setConfiguration(config);
			matches.push_back(c);
		}
		catch (...) {}
    }

    return matches;
}

CameraFactory * CameraFactory::getInstance()
{
    if( CameraFactory::instance == NULL )
	CameraFactory::instance = new CameraFactory();

    return CameraFactory::instance;
};

void CameraFactory::printDetails(bool state)
{
    CameraFactory *f = CameraFactory::getInstance();
    std::vector<Camera *>cameras = f->getCameras();

    wclcout << cameras.size() << " Camera(s) Found" <<endl;

    if ( cameras.size() ){
	wclcout << "-----------------------------------------------------" <<endl;

	for(std::vector<Camera *>::iterator it = cameras.begin();
	    it != cameras.end();
	    ++it ) {
	    Camera *c = *it;
	    c->printDetails(state);
	}

	wclcout << "-----------------------------------------------------" <<endl;
    }
}


}; //namespace
