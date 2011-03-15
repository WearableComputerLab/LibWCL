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
#include <flycapture/FlyCapture2.h>
#include <wcl/camera/CameraException.h>
#include <wcl/camera/PTGreyCameraFactory.h>

using namespace FlyCapture2;

namespace wcl {

    PTGreyCameraFactory *PTGreyCameraFactory::instance;
    std::vector<PTGreyCamera *> PTGreyCameraFactory::cameras;

    PTGreyCameraFactory::PTGreyCameraFactory()
    {}

    PTGreyCameraFactory::~PTGreyCameraFactory()
    {
	for(std::vector<PTGreyCamera *>::iterator it = this->cameras.begin();
	    it != this->cameras.end();
	    ++it )
	{
	    PTGreyCamera *c = *it;
	    delete c;
	}
    }

    std::vector<PTGreyCamera *> PTGreyCameraFactory::getCameras()
    {
	PTGreyCameraFactory *instance = PTGreyCameraFactory::getInstance();
	return instance->cameras;
    }

    void PTGreyCameraFactory::probeCameras()
    {
	BusManager busMgr;
	CameraInfo camInfo[128];
	unsigned int numCameras = 128;
	Error error;

	error = BusManager::DiscoverGigECameras(camInfo, &numCameras);
	if( error != PGRERROR_OK ){
	    throw CameraException("PROBE ISSUE1");
	}

	for( unsigned i = 0; i < numCameras; i++ ){
	    PGRGuid guid;
	    error = busMgr.GetCameraFromIndex(i, &guid );
	    if( error  != PGRERROR_OK )
		throw CameraException("PROBE ISSUE2");

	    try {
		PTGreyCamera *camera = new PTGreyCamera(guid);
		this->cameras.push_back(camera);
	    } catch (CameraException e ) {
		throw CameraException(e.what());
	    }

	}

    }

    PTGreyCameraFactory *PTGreyCameraFactory::getInstance()
    {
	if( PTGreyCameraFactory::instance == NULL ){
	    PTGreyCameraFactory::instance = new PTGreyCameraFactory();
	    PTGreyCameraFactory::instance->probeCameras();
	}

	return PTGreyCameraFactory::instance;
    }

} // namespace wcl
