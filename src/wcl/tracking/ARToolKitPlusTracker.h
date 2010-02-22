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

#ifndef WCL_TRACKING_ARTOOLKITPLUSTRACKER_H
#define WCL_TRACKING_ARTOOLKITPLUSTRACKER_H

#include <stdint.h>
#include <vector>
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>

#include <wcl/camera/Camera.h>
#include <wcl/tracking/Tracker.h>
#include <wcl/tracking/ARToolKitPlusTrackedObject.h>

/**
 * The maximum amount of markers that are detected in any one frame
 */
#ifndef WCL_ARTOOLKITPLUSTRACKER_MARKER_DETECTION_COUNT
#define WCL_ARTOOLKITPLUSTRACKER_MARKER_DETECTION_COUNT 8
#endif

namespace wcl
{

/**
 * The ARToolKitPlusTracker class integrates ARToolKitPlus
 * with libWCL. By default ARToolkitPlus is setup to use BCH markers (up to
 * 1024) and detects a maximum of WCL_ARTOOLKITPLUSTRACKER_MARKER_DETECTION_COUNT marker
 */
class ARToolKitPlusTracker: public Tracker
{
public:
    ARToolKitPlusTracker( const unsigned markerWidth = 80,
                          const int thresholdValue = -1 /* -1 = AUTO */,
                          const screenWidth = 800,   /* ScreenWidth/screenHeight are used for getProjectionMatrix */
                          const screenHeight = 600);
    virtual ~ARToolKitPlusTracker();
    virtual void setCamera(Camera *);

    virtual void update();
    virtual TrackedObject* getObject(const std::string name);
    virtual std::vector<TrackedObject *> getAllObjects();
    virtual void setUnits(const Units u);
    SMatrix getProjectionMatrix();
    SMatrix getModelViewMatrix();

    /**
     * Set the threshold to use when processing images. A value of
     * <=-1 indicates automatic thresholding should be used
     */
    void setThreshold( const int value );

    /**
     * Give details about the tracker, version of artoolKitplus
     * and the amount of markers, etc
     */
    void toString();

private:
    std::vector<ARToolKitPlusTrackedObject *>objects;
    std::map<unsigned, ARToolKitPlusTrackedObject *> mapping;

    ARToolKitPlus::TrackerSingleMarker *tracker;
    unsigned markerWidth;
    Camera *camera;

    float confidence;
    int bestMarker;
    int markersFound;
};

};

#endif
