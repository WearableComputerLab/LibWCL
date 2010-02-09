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

int Camera::convertPixelYUVtoRGB(const int y, const int u, const int v )
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;

    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));

    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;

    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;

    return pixel32;
}

void Camera::convertImageYUVtoRGB(const unsigned char *yuv, unsigned char *rgb,
			 const unsigned int width, const unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;

    for(in = 0; in < width * height * 2; in += 4)
    {
	pixel_16 =
	    yuv[in + 3] << 24 |
	    yuv[in + 2] << 16 |
	    yuv[in + 1] <<  8 |
	    yuv[in + 0];

	y0 = (pixel_16 & 0x000000ff);
	u  = (pixel_16 & 0x0000ff00) >>  8;
	y1 = (pixel_16 & 0x00ff0000) >> 16;
	v  = (pixel_16 & 0xff000000) >> 24;

	pixel32 = Camera::convertPixelYUVtoRGB(y0, u, v);
	pixel_24[0] = (pixel32 & 0x000000ff);
	pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
	pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

	rgb[out++] = pixel_24[0];
	rgb[out++] = pixel_24[1];
	rgb[out++] = pixel_24[2];

	pixel32 = Camera::convertPixelYUVtoRGB(y1, u, v);
	pixel_24[0] = (pixel32 & 0x000000ff);
	pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
	pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

	rgb[out++] = pixel_24[0];
	rgb[out++] = pixel_24[1];
	rgb[out++] = pixel_24[2];
    }
}

}
