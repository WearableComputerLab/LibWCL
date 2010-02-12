/*-
 * Copyright (c) 2009 Benjamin Close <Benjamin.Close@clearchain.com>
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

#include "ARToolKitPlusTracker.h"
#include <assert.h>
#include <ARToolKitPlus/CameraImpl.h>

using namespace ARToolKitPlus;

namespace wcl {


ARToolKitPlusTracker::ARToolKitPlusTracker( const unsigned width, const unsigned height):
    markerWidth(width),c_ptr(NULL)
{

    this->c_ptr = new ARToolKitPlus::CameraImpl;

    // create a tracker that does:
    //  - 6x6 sized marker images (in pixel)
    //  - samples at a maximum of 6x6
    //  - works with luminance (gray) images
    //  - can load a maximum of 1 pattern
    //  - can detect a maximum of 8 patterns in one image


    // TODO XXX Handle camera size changes (This class shouldn't need a width  height ) - benjsc 20100201
    this->tracker= new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 1, 8>(width,height);
    this->tracker->setPatternWidth(80);

    // We use BCH images which have a thinner border
    this->tracker->setBorderWidth (0.125f);

    this->tracker->setThreshold(150);
    this->tracker->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);

    // Use LUT lookups to speed up calculations, these only works with images up to 1024x1024
    if( width < 1024 && height < 1024 )
	this->tracker->setUndistortionMode(ARToolKitPlus::UNDIST_LUT);

    // Create 1024 base markers as we use BCH.
    for(unsigned i=0; i < 1024; i++ ){
	ARToolKitPlusTrackedObject *o = new ARToolKitPlusTrackedObject(width, height, i);
	this->objects.push_back(o);
    }
}

ARToolKitPlusTracker::~ARToolKitPlusTracker()
{

    // ARToolkitPlus deletes the camera hence we don't
    // need to call: delete this->c_ptr as it's already done and
    // would cause a SIGSEGV if we try it
    delete this->tracker;

    for(std::vector<ARToolKitPlusTrackedObject *>::iterator it = this->objects.begin();
	it != this->objects.end();
	++it ){
	ARToolKitPlusTrackedObject *object= *it;
	delete object;
    }
}

void ARToolKitPlusTracker::setCamera(Camera *camera)
{
    assert( camera != NULL && "You Can't specifiy NULL for the camera");
    this->camera = camera;
    ARToolKitPlus::PIXEL_FORMAT format;

    // Set the format based on what the camera is using
    switch (this->camera->getImageFormat()){
	case Camera::MJPEG:
	case Camera::YUYV422:
        default:
            printf("ARToolKitPlusTracker::Unknown/Unsupported Camera Image Format Assuming RGB\n");
            /* Fallthrough */
	case Camera::RGB8:
            format=ARToolKitPlus::PIXEL_FORMAT_RGB;
            break;
	case Camera::BGR8:
            format=ARToolKitPlus::PIXEL_FORMAT_BGR;
            break;
    }

    // Create a new ARToolkit Camera to
    // init the tracker
    c_ptr->xsize=this->camera->getFormatWidth();
    c_ptr->ysize=this->camera->getFormatHeight();
    Camera::Distortion d = this->camera->getDistortion();
    c_ptr->mat[0][0]=d.cameraToWorld[0][0];
    c_ptr->mat[0][1]=d.cameraToWorld[1][0];
    c_ptr->mat[0][2]=d.cameraToWorld[2][0];
    c_ptr->mat[1][0]=d.cameraToWorld[0][1];
    c_ptr->mat[1][1]=d.cameraToWorld[1][1];
    c_ptr->mat[1][2]=d.cameraToWorld[2][1];
    c_ptr->mat[2][0]=d.cameraToWorld[0][2];
    c_ptr->mat[2][1]=d.cameraToWorld[1][2];
    c_ptr->mat[2][2]=d.cameraToWorld[2][2];
    c_ptr->mat[3][0]=d.cameraToWorld[0][3];
    c_ptr->mat[3][1]=d.cameraToWorld[1][3];
    c_ptr->mat[3][2]=d.cameraToWorld[2][3];
    c_ptr->dist_factor[0]=d.factor[0];
    c_ptr->dist_factor[1]=d.factor[1];
    c_ptr->dist_factor[2]=d.factor[2];
    c_ptr->dist_factor[3]=d.factor[3];

    this->tracker->setPixelFormat(format);
    this->tracker->setCamera(c_ptr, 1.0, 1000.0f);
    this->tracker->init(NULL, 1.0, 1000.0f);
}

void ARToolKitPlusTracker::update()
{
    int detected, i=0;
    ARMarkerInfo *markers;
    ARFloat conv[3][4];
    ARFloat center[2]={0.0,0.0};

    detected=this->tracker->calc(this->camera->getFrame());
//    this->tracker->calc(this->camera->getFrame(), -1, false, NULL, &detected);

    printf("%d detected\n",detected);
    while( i < detected ){
	SMatrix m(4);
	m.storeIdentity();
	this->tracker->executeSingleMarkerPoseEstimator(&markers[i],
							center, // XXX Hard coded as per seinglImpl
							(ARFloat)80, //XXX Hard coded as per SinglImpl
							conv );

	for(std::vector<ARToolKitPlusTrackedObject *>::iterator it = this->objects.begin();
	    it != this->objects.end();
	    ++it ){

	    ARToolKitPlusTrackedObject *marker=*it;

	    if ( marker->getID() == markers[i].id ){


		for(unsigned row=0; row < 3; row++ )
		    for(unsigned c =0; c < 4; c++)
			m[row][c]=conv[row][c];

		marker->setTransform(m);
	    }
	}
	i++;
    }
}

TrackedObject* ARToolKitPlusTracker::getObject(const std::string name)
{
    for(std::vector<ARToolKitPlusTrackedObject *>::iterator it;
	it != this->objects.end();
	++it )
    {
	ARToolKitPlusTrackedObject *temp = *it;
	if(temp->getName()==name)
	    return temp;
    }

    return NULL;
}

std::vector<TrackedObject *> ARToolKitPlusTracker::getAllObjects()
{
    std::vector<TrackedObject *> v;
    for(std::vector<ARToolKitPlusTrackedObject *>::iterator it =
	this->objects.begin();
	it!=this->objects.end();
	++it)
	v.push_back(*it);
    return v;
}

void ARToolKitPlusTracker::setUnits(Units u)
{
#warning "ARToolKitPlusTracker::setUnits Is not Implemented"
    /// XXX This should be possible provided we know the
    /// marker size - benjsc 20100201
    /*
    switch(u)
    {
        case INCHES:
	    ;
        case CM:
	    ;
        case MM:
	    ;
    ;

    /// XXX Implement Me
    */
}


void ARToolKitPlusTracker::toString()
{
    const char* description = this->tracker->getDescription();
    printf("ARToolKitPlus compile-time information:\n%s\n\n", description);
    //TODO Add marker count
}

};
