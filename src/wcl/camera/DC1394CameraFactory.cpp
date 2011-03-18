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

#include "CameraException.h"
#include "DC1394CameraFactory.h"

namespace wcl {

	DC1394CameraFactory *DC1394CameraFactory::instance;
	std::vector<DC1394Camera *> DC1394CameraFactory::cameras;

	DC1394CameraFactory::DC1394CameraFactory()
	{
	}

	DC1394CameraFactory::~DC1394CameraFactory()
	{
		for(std::vector<DC1394Camera *>::iterator it = this->cameras.begin();
				it != this->cameras.end();
				++it )
		{
			DC1394Camera *c = *it;
			delete c;
		}
	}

	DC1394CameraFactory *DC1394CameraFactory::getInstance()
	{
		if( DC1394CameraFactory::instance == NULL ){
			DC1394CameraFactory::instance = new DC1394CameraFactory();
			instance->probeCameras();
		}

		return DC1394CameraFactory::instance;
	}


	std::vector<DC1394Camera *> DC1394CameraFactory::getCameras()
	{
		DC1394CameraFactory *instance = DC1394CameraFactory::getInstance();
		return instance->cameras;
	}


	void DC1394CameraFactory::probeCameras()
	{
		using namespace std;

		// attempt to located the cameras on the firewire bus
		dc1394_t * d;
		dc1394camera_list_t * list;
		d = dc1394_new ();
		if( !d )
		    throw CameraException(CameraException::EACCESS);

		if(dc1394_camera_enumerate (d, &list) !=  DC1394_SUCCESS )
		    throw CameraException(CameraException::CONNECTIONISSUE);

		for( unsigned i = 0 ; i < list->num; i++ ){
			DC1394Camera *c = new DC1394Camera(list->ids[i].guid);
			this->cameras.push_back(c);
		}

		dc1394_camera_free_list(list);
	}

}
