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
#include <ARToolKitPlus/CameraAdvImpl.h>

using namespace ARToolKitPlus;

namespace wcl {


ARToolKitPlusTracker::ARToolKitPlusTracker( const unsigned iscreenWidth, const unsigned iscreenHeight,
					    const unsigned imarkerWidth, const int thresholdValue ):
    markerWidth(imarkerWidth)
{

    // create a tracker that does:
    //  - 6x6 sized marker images (in bch pixel) - we hard code this as 1024 images is huge
    //  - samples at a maximum of 6x6 -??? Seems to relate to image division maximum
    //  - can load a maximum of 0 pattern - not required as we use BCH patterns
    //  - can detect a maximum of imaxMarkserPerImage patterns in one image
    this->tracker=
	new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 0, WCL_ARTOOLKITPLUSTRACKER_MARKER_DETECTION_COUNT>(iscreenWidth, iscreenHeight);

    this->tracker->setPatternWidth(imarkerWidth);

    // Use LUT lookups to speed up calculations, these only works with images up to 1024x1024
    if( iscreenWidth < 1024 && iscreenHeight < 1024 )
	this->tracker->setUndistortionMode(ARToolKitPlus::UNDIST_LUT);

    // Use binary encoded markers
    this->tracker->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);

    // We use BCH images which have a thinner border
    this->tracker->setBorderWidth (0.125f);

    // Set the threshold to either a user value or auto by default
    this->setThreshold( thresholdValue );

    // Use the updated pose estimator (Robust Pose Estimation From a Planar Target)
    this->tracker->setPoseEstimator(POSE_ESTIMATOR_RPP);

    // Create 1024 base markers as we use BCH, and also populate the map
    for(unsigned i=0; i < 1024; i++ ){
	ARToolKitPlusTrackedObject *o = new ARToolKitPlusTrackedObject(imarkerWidth, i);
	this->objects.push_back(o);

	this->mapping[i] = o;
    }
}

ARToolKitPlusTracker::~ARToolKitPlusTracker()
{

    // ARToolkitPlus deletes the last camera assigned hence we don't
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
    //assert( camera->hasDistortionMatrix() != false && "You must have a valid distortion matrix for the camera before ARToolKitPlusTracker will work");

    this->camera = camera;

    ARToolKitPlus::PIXEL_FORMAT format;

    // Set the format based on what the camera is using
    switch (this->camera->getImageFormat()){
	case Camera::MJPEG:
	case Camera::YUYV422:
        default:
            printf("ARToolKitPlusTracker:SetCamera: Unknown/Unsupported Camera Image Format Assuming RGB8\n");
            /* Fallthrough */
	case Camera::RGB8:
            format=ARToolKitPlus::PIXEL_FORMAT_RGB;
            break;
	case Camera::BGR8:
            format=ARToolKitPlus::PIXEL_FORMAT_BGR;
	case Camera::MONO8:
	    format=ARToolKitPlus::PIXEL_FORMAT_LUM;
            break;
    }

    // Create a new ARToolkit Camera to
    // init the tracker, not ARToolKitPlus will remove
    // delete the last camera hence we never need to free this camera
    ARToolKitPlus::Camera *c_ptr = new ARToolKitPlus::CameraImpl;
    /*
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
    */

    c_ptr->xsize=320;
    c_ptr->ysize=240;
    c_ptr->mat[0][0]=406.040405;
    c_ptr->mat[0][1]=0.;
    c_ptr->mat[0][2]=154.0;
    c_ptr->mat[0][3]=0.;
    c_ptr->mat[1][0]=0.;
    c_ptr->mat[1][1]=404.388489;
    c_ptr->mat[1][2]=115;
    c_ptr->mat[1][3]=0;
    c_ptr->mat[2][0]=0.;
    c_ptr->mat[2][1]=0.;
    c_ptr->mat[2][2]=1.;
    c_ptr->mat[2][3]=0.;
    c_ptr->dist_factor[0]=159.;
    c_ptr->dist_factor[1]=139.;
    c_ptr->dist_factor[2]=-84.9000015;
    c_ptr->dist_factor[3]=0.979329705;

    /*
    ARToolKitPlus::CameraAdvImpl *c_ptr = new ARToolKitPlus::CameraAdvImpl;
    c_ptr->cc[0]=(ARFloat)320;
    c_ptr->cc[1]=(ARFloat)240;
    c_ptr->fc[0]=(ARFloat)1500.0;
    c_ptr->fc[1]=(ARFloat)1500.0;
    for(int i=0; i< 6;i++)
	c_ptr->kc[i]=(ARFloat)0.0;
    c_ptr->undist_iterations=0;
    */

    this->tracker->setPixelFormat(format);
    this->tracker->init(NULL, 100.0, 10000.0f);
    this->tracker->setCamera(c_ptr, 100.0, 10000.0f);
}

void ARToolKitPlusTracker::update()
{
    int i=0;
    ARMarkerInfo *markers;
    ARFloat conv[3][4];
    ARFloat center[2]={0.0,0.0};// XXX Hard coded as per ARToolKitPlus::TrackerSingleMarkerImpl.cxx

    this->bestMarker=this->tracker->calc(this->camera->getFrame(), -1, true, &markers, &this->markersFound);
    this->confidence = (float)tracker->getConfidence();


    // set all markers to not visable
    for(std::vector<ARToolKitPlusTrackedObject *>::iterator it = this->objects.begin();
	it != this->objects.end();
	++it ){
	ARToolKitPlusTrackedObject *marker=*it;
	marker->setVisible(false);
    }

    // Update the found markers
    while( i < this->markersFound ){
	SMatrix m(4);
	m.storeIdentity();
	this->tracker->executeSingleMarkerPoseEstimator(&markers[i],
							center,
							this->markerWidth,
							conv );



	if( markers[i].id > -1 ){
	    ARToolKitPlusTrackedObject *marker = this->mapping[markers[i].id];
	    for(unsigned row=0; row < 3; row++ )
		for(unsigned c =0; c < 4; c++)
		    m[c][row]=conv[row][c];

		marker->setTransform(m);
		marker->setVisible(true);
	}
	i++;
    }
}

TrackedObject* ARToolKitPlusTracker::getObject(const std::string name)
{
    for(std::vector<ARToolKitPlusTrackedObject *>::iterator it=this->objects.begin();
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
	if( (*it)->isVisible())
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

void ARToolKitPlusTracker::setThreshold(const int value )
{
    if( value < 0 )
	this->tracker->activateAutoThreshold(true);
    else {
	this->tracker->activateAutoThreshold(false);
	this->tracker->setThreshold(value);
    }
}


void ARToolKitPlusTracker::toString()
{
    const char* description = this->tracker->getDescription();
    printf("ARToolKitPlus compile-time information:\n%s\n\n", description);
    printf("Thresholding: %s, current value %d\n",
	   this->tracker->isAutoThresholdActivated() ? "AUTO": "MANUAL",
	   this->tracker->getThreshold());
    printf("%d Markers found, %d is the id of best marker with a %f confidence\n",
	   this->markersFound,
	   this->bestMarker,
	   this->confidence);
    if( this->markersFound > 0 ){
	printf("Best Marker Matrix:\n");
	for(int i=0; i<16; i++)
	    printf("%.4f  %s", tracker->getModelViewMatrix()[i], (i%4==3)?"\n  " : "");
    }
}

/**
 * Get the projection matrix ARToolkit is using
 */
SMatrix ARToolKitPlusTracker::getProjectionMatrix()
{
    SMatrix m(4);
    const ARFloat *f = this->tracker->getProjectionMatrix();
    int i=0;
    for( int r=0; r<4;r++ ){
	for ( int c=0; c<4; c++)
	    m[r][c]=f[i++];

    }

    return m;
}

/**
 * Get the model view matrix of the best marker
 *
 * @return The model view matrix of the best marker
 */
SMatrix ARToolKitPlusTracker::getModelViewMatrix()
{
    SMatrix m(4);
    const ARFloat *f = this->tracker->getModelViewMatrix();
    int i=0;
    for( int r=0; r<4;r++ ){
	for ( int c=0; c<4; c++)
	    m[r][c]=f[i++];

    }

    return m;
};

}; // namespace wcl
