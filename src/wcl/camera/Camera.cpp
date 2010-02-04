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

#include "Camera.h"

namespace wcl
{

CameraBuffer::CameraBuffer():
    start(0)
{}



Camera::Camera() :
    buffers(NULL),
    bufferSize(0),
    numBuffers(0)
{
    distortion.cameraToWorld.storeIdentity();
}

Camera::~Camera()
{
    this->destroyBuffers();
}

void Camera::allocateBuffers(const size_t size, const unsigned count)
{
    this->destroyBuffers();
    this->buffers = new CameraBuffer[count];

}

void Camera::destroyBuffers()
{
    if( this->buffers )
        delete [] this->buffers;

    this->numBuffers=0;
}

void Camera::setFormat(const ImageFormat f, const unsigned width, const unsigned height)
{
    this->format = f;
    this->width = width;
    this->height = height;
}

Camera::Distortion Camera::getDistortion() const
{
    return this->distortion;
}

}
