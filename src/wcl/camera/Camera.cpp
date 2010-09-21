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

#include <assert.h>
#include <config.h>
#include <iostream>
#include "Camera.h"
#include "CameraException.h"

#if ENABLE_VIDEO
    #include <video/VideoDecoder.h>
#endif

using namespace std;

namespace wcl
{
	struct Camera::Priv
	{
#if ENABLE_VIDEO
	VideoDecoder *decoder;
#endif
	};

	Camera::CameraBuffer::CameraBuffer():
		start(0)
	{}



	Camera::Camera() :
		buffers(NULL),
		bufferSize(0),
		numBuffers(0),
		conversionBuffer(NULL),
		areParametersSet(false),
		internal(NULL)
	{
	}

	Camera::~Camera()
	{
		this->destroyBuffers();
		if(this->conversionBuffer != NULL){
			delete (unsigned char *)this->conversionBuffer->start;
			delete this->conversionBuffer;
		}
		if(this->internal){
#if ENABLE_VIDEO
		    delete this->internal->decoder;
#endif
		    delete this->internal;
		}
	}

	void Camera::allocateBuffers(const size_t size, const unsigned count)
	{
		this->destroyBuffers();
		this->buffers = new CameraBuffer[count];
		for(unsigned i =0; i < count; i++)
			this->buffers[i].length = size;
	}

	void Camera::destroyBuffers()
	{
		if( this->buffers )
			delete [] this->buffers;

		this->buffers = NULL;
		this->numBuffers=0;
	}

	Camera::CameraParameters Camera::getParameters() const
	{
		return this->parameters;
	}

	int Camera::convertPixelYUYV422toRGB8(const int y, const int u, const int v )
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

	void Camera::convertImageYUYV422toRGB8(const unsigned char *yuv, unsigned char *rgb,
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

			pixel32 = Camera::convertPixelYUYV422toRGB8(y0, u, v);
			pixel_24[0] = (pixel32 & 0x000000ff);
			pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
			pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

			rgb[out++] = pixel_24[0];
			rgb[out++] = pixel_24[1];
			rgb[out++] = pixel_24[2];

			pixel32 = Camera::convertPixelYUYV422toRGB8(y1, u, v);
			pixel_24[0] = (pixel32 & 0x000000ff);
			pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
			pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

			rgb[out++] = pixel_24[0];
			rgb[out++] = pixel_24[1];
			rgb[out++] = pixel_24[2];
		}
	}

	void Camera::convertImageMONO8toRGB8( const unsigned char *mono, unsigned char *rgb,
			const unsigned int width, const unsigned int height )
	{
		unsigned int in, out=0;
		for(in = 0; in < width * height; in++ )
		{
			rgb[out+0]=mono[in];
			rgb[out+1]=mono[in];
			rgb[out+2]=mono[in];
			out+=3;
		}
	}

	void Camera::convertImageRGB8toMONO8(const unsigned char* rgb, unsigned char* mono, const unsigned width, const unsigned height)
	{
		unsigned int in, out=0;
		for (out = 0; out < width*height; ++out)
		{
			mono[out] = (unsigned char) (rgb[in]*0.3 + rgb[in+1]*0.59 + rgb[in+2]*0.11);
			in+=3;
		}
	}

	void Camera::setConfiguration(Configuration c)
	{
		assert (c.width != 0);
		assert (c.height != 0);
		assert (c.fps != 0);

		this->activeConfiguration = c;
	}

	std::vector<Camera::Configuration> Camera::getSupportedConfigurations() const
	{
		return this->supportedConfigurations;
	}


	Camera::Configuration Camera::findConfiguration(Camera::Configuration partialConfig) const
	{
		for (std::vector<Configuration>::const_iterator it = supportedConfigurations.begin(); it < supportedConfigurations.end(); ++it)
		{
			// does this config match what we are looking for?
			if (partialConfig.format != ANY && (*it).format != partialConfig.format)
				continue;

			if (partialConfig.width != 0 && (*it).width != partialConfig.width)
				continue;

			if (partialConfig.height != 0 && (*it).height != partialConfig.height)
				continue;

			if (partialConfig.fps != 0 && (*it).fps != partialConfig.fps)
				continue;

			// matches everything we asked for!
			return *it;
		}
		throw CameraException(CameraException::INVALIDCONFIGURATION);
	}


	/**
	 * Perform a software conversion of the frame to the requested format.
	 * The first call to this function is expensive as an internal buffer must be
	 * setup to support the conversion. Successive calls with the same format
	 * only incur the performance hit of the conversion. Changing image formats
	 * will also incurr an reallocation performance hit.
	 *
	 * @param f The format to convert the frame too
	 * @return A pointer to the converted frame
	 */

	const unsigned char* Camera::getFrame()
	{
		update();
		return currentFrame;
	}

	const unsigned char *Camera::getFrame(const ImageFormat f )
	{
		update();

		// Handle the same image format being requested
		if( this->activeConfiguration.format == f )
			return currentFrame;

		unsigned width = this->activeConfiguration.width;
		unsigned height = this->activeConfiguration.height;

		this->setupConversionBuffer(this->getFormatBufferSize(f));
		unsigned char *buffer=(unsigned char *)this->conversionBuffer->start;

		switch( f ){
			case RGB8:
				{
					switch( this->activeConfiguration.format){
						case MONO8:
							convertImageMONO8toRGB8(currentFrame, buffer, width, height);
							return buffer;

						case YUYV422:
							convertImageYUYV422toRGB8( currentFrame, buffer, width, height);
							return buffer;

#if ENABLE_VIDEO
						case MJPEG:{
						    // Init the video decoder on the first MJPEG decoding frame
						    if( this->internal == NULL){
							this->internal = new Priv;
							this->internal->decoder = new VideoDecoder(width, height,CODEC_ID_MJPEG, false );
						    }
						    internal->decoder->nextFrame(currentFrame, this->getFormatBufferSize());
						    return internal->decoder->getFrame();
						}
#endif
						default:
							;
					}
					goto NOTIMP;
				}
			case MJPEG:
			case YUYV422:
			case YUYV411:
			case RGB16:
			case RGB32:
			case BGR8:
			case MONO8:
			case MONO16:
			default:
NOTIMP:
				assert(0 && "Camera::getFrame(const ImageFormat) - Requested Conversion Not Implemented");
		}

		return NULL;
	}


	const unsigned char* Camera::getCurrentFrame() const
	{
		return currentFrame;
	}

	void Camera::getCurrentFrame(unsigned char* buffer, const ImageFormat& format) const
	{
		// Handle the same image format being requested
		if( this->activeConfiguration.format == format )
		{
			memcpy(buffer, currentFrame, getFormatBufferSize());
			return;
		}

		unsigned width = this->activeConfiguration.width;
		unsigned height = this->activeConfiguration.height;

		unsigned char* temp;

		switch( format ){
			case RGB8:
				{
					switch( this->activeConfiguration.format){
						case MONO8:
							convertImageMONO8toRGB8(currentFrame, buffer, width, height);
							return;

						case YUYV422:
							convertImageYUYV422toRGB8( currentFrame, buffer, width, height);
							return;

#if ENABLE_VIDEO
						case MJPEG:{
						    // Init the video decoder on the first MJPEG decoding frame
							VideoDecoder decoder(width, height,CODEC_ID_MJPEG, false );
						    decoder.nextFrame(currentFrame, this->getFormatBufferSize());
							memcpy(buffer, decoder.getFrame(), getFormatBufferSize(format));
							return;
						}
#endif
						default:
							;
					}
					goto NOTIMP;
				}
			case MJPEG:
			case YUYV422:
			case YUYV411:
			case RGB16:
			case RGB32:
			case BGR8:
			case MONO8:
				switch (this->activeConfiguration.format)
				{
					case RGB8:
						convertImageRGB8toMONO8(currentFrame, buffer, width, height);
						return;

					case YUYV422:
						//create a temporary buffer
						temp = new unsigned char[getFormatBufferSize(RGB8)];
						convertImageYUYV422toRGB8(currentFrame, temp, width, height);
						convertImageRGB8toMONO8(temp, buffer, width, height);
						delete [] temp;
						return;

					case MJPEG:
						{
						VideoDecoder dec(width, height,CODEC_ID_MJPEG, false );
						dec.nextFrame(currentFrame, this->getFormatBufferSize());
						convertImageRGB8toMONO8(dec.getFrame(), buffer, width, height);
						return;
						}
					default:
						;
				}
				goto NOTIMP;
				break;

			case MONO16:
			default:
NOTIMP:
				assert(0 && "Camera::getFrame(const ImageFormat) - Requested Conversion Not Implemented");
		}
	}

	unsigned Camera::getFormatBytesPerPixel() const
	{
		return Camera::getFormatBytesPerPixel(this->activeConfiguration.format);
	}

	unsigned Camera::getFormatBytesPerPixel(const ImageFormat f ) const
	{
		switch( f ){
			case RGB8:
				return 3;
			case RGB16:
				return 6;
			case RGB32:
				return 12;
			case BGR8:
				return 3;
			case MONO8:
				return 1;
			case MONO16:
				return 2;
			case YUYV422:
				return 4;
			case YUYV411:
				return 4;
			case MJPEG:
				return 12;
		}
	}

	unsigned Camera::getFormatBufferSize() const
	{
		return  Camera::getFormatBufferSize( this->activeConfiguration.format);
	}

	unsigned Camera::getFormatBufferSize(const ImageFormat f ) const
	{
		return (this->getFormatBytesPerPixel(f) *
				this->getActiveConfiguration().width *
				this->activeConfiguration.height);
	}

	void Camera::setupConversionBuffer( const size_t buffersize )
	{
		if( this->conversionBuffer ){
			if( this->conversionBuffer->length == buffersize )
				return;

			delete this->conversionBuffer;
		}

		this->conversionBuffer = new CameraBuffer;
		this->conversionBuffer->length = buffersize;
		this->conversionBuffer->start = (void *)new unsigned char[buffersize];
	}

	bool Camera::hasParameters() const { return areParametersSet; }

	void Camera::setParameters(const Camera::CameraParameters& p) {
		areParametersSet = true;
		this->parameters = p;
	}

	void Camera::printDetails(bool state)
	{

	    Configuration a = this->getActiveConfiguration();
	    cout << "Camera: " << this->id << " (" << this->getTypeIdentifier() << ")"
		 << this->imageFormatToString(a.format) << ":" << a.width << "x" << a.height << "@" << a.fps << endl;

	    if ( state ){
		cout << "Features/Modes" << endl;
		for(std::vector<Configuration>::iterator it =
		    supportedConfigurations.begin(); it !=
		    supportedConfigurations.end(); ++it ){

		    Configuration c = *it;
		    cout << "\t" << this->imageFormatToString(c.format) << " "
			 << c.width << "x" << c.height << " @" << c.fps << endl;
		}
	    }
	}

	const char *Camera::imageFormatToString(const ImageFormat f )
	{
	    switch(f)
	    {
		case MJPEG: return "MJPEG";
		case YUYV422: return "YUYV422";
		case YUYV411: return "YUYV411";
		case RGB8: return "RGB8";
		case RGB16: return "RGB16";
		case RGB32: return "RGB32";
		case BGR8: return "BGR8";
		case MONO8: return "MONO8";
		case MONO16: return "MONO16";
		case ANY:
		default:
		    return "UNKNOWN:";
	    };
	}

}


