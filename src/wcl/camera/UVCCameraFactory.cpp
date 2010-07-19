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

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <wcl/camera/UVCCameraFactory.h>

#define MAX_DEVICES 64

namespace wcl {

UVCCameraFactory *UVCCameraFactory::instance;
std::vector<UVCCamera *> UVCCameraFactory::cameras;

UVCCameraFactory::UVCCameraFactory()
{}

UVCCameraFactory::~UVCCameraFactory()
{
    for(std::vector<UVCCamera *>::iterator it = this->cameras.begin();
	it != this->cameras.end();
	++it )
    {
	UVCCamera *c = *it;
	delete c;
    }
}

UVCCameraFactory *UVCCameraFactory::getInstance()
{
    if( UVCCameraFactory::instance == NULL ){
	UVCCameraFactory::instance = new UVCCameraFactory();
	UVCCameraFactory::instance->probeCameras();
    }

    return UVCCameraFactory::instance;
}


std::vector<UVCCamera *> UVCCameraFactory::getCameras()
{
    UVCCameraFactory *instance = UVCCameraFactory::getInstance();
    return instance->cameras;
}


void UVCCameraFactory::probeCameras()
{
    // attempt to located the cameras on the USB Bus
    // to do this we walk throught the device nodes in /dev
    // until we get a node we can't open/doesn't exist
    for( unsigned i=0; i < MAX_DEVICES; i++){
	std::stringstream device;
	device<<"/dev/video";
	device<<i;
	try {
	    UVCCamera *c = new UVCCamera( device.str().c_str());
	    this->cameras.push_back( c );
	} catch (std::string s )
	{
	    printf("Exception Raised: %s\n", s.c_str());
	    // Exception raised, hence there's no device, or some other issue
	    // we simply break the loop at this point
	    break;
	}

    }
}

}
