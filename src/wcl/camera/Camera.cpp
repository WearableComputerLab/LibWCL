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
#include <string.h>
#include <algorithm>
#include "IO.h"
#include "Camera.h"
#include "CameraException.h"

#if ENABLE_VIDEO
    #include <video/VideoDecoder.h>
#endif

using namespace std;


namespace wcl
{
/**
 * Helper function to sort the configuration list
 */

static bool configurationSort ( Camera::Configuration a, Camera::Configuration b)
{
	if(((int)a.format) > (int)b.format)
		return true;
	if(((int)a.format) < (int)b.format)
		return false;

	// Format Equal
	if(a.format == Camera::FORMAT7){
		if ( ((int)a.format7.format) > ((int)b.format7.format))
			return true;
		if ( ((int)a.format7.format) < ((int)b.format7.format))
			return false;

		// Format Equal
		if( a.format7.xMax > b.format7.xMax )
			return true;
		else
			return false;
	} else {
		if ( a.width > b.width )
			return true;
		if( a.width < b.width )
			return false;
		// Width Equal
		if ( a.height > b.height )
			return true;
		if( a.height < b.height )
			return false;
		// Height Equal
		if( a.fps > b.fps )
			return true;

		// Equal or < fps
		return false;
	}
}


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
		areParametersSet(false),
		currentFrame(NULL),
		conversionBuffer(NULL),
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

	void Camera::convertImageYUYV411toRGB8(const unsigned char *yuv, unsigned char *rgb,
			const unsigned int width, const unsigned int height)
	{
		unsigned int in = 0;
		unsigned int out = 0;
		unsigned char pixel_24[3];
		unsigned int pixel32;
		int y1, y2, y3, y4, u, v;

		for(in = 0; in < (6 * width * height / 4); in += 6)
		{
			u = yuv[in];
			y1 = yuv[in+1];
			y2 = yuv[in+2];
			v = yuv[in+3];
			y3 = yuv[in+4];
			y4 = yuv[in+5];
			
			//Convert formats and push into chars. 
			pixel32 = Camera::convertPixelYUYV422toRGB8(y1, u, v);
			pixel_24[0] = (pixel32 & 0x000000ff);
			pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
			pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

			//Push out. 
			rgb[out++] = pixel_24[0];
			rgb[out++] = pixel_24[1];
			rgb[out++] = pixel_24[2];

			//Convert formats and push into chars. 
			pixel32 = Camera::convertPixelYUYV422toRGB8(y2, u, v);
			pixel_24[0] = (pixel32 & 0x000000ff);
			pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
			pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

			//Push out. 
			rgb[out++] = pixel_24[0];
			rgb[out++] = pixel_24[1];
			rgb[out++] = pixel_24[2];

			//Convert formats and push into chars. 
			pixel32 = Camera::convertPixelYUYV422toRGB8(y3, u, v);
			pixel_24[0] = (pixel32 & 0x000000ff);
			pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
			pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

			//Push out. 
			rgb[out++] = pixel_24[0];
			rgb[out++] = pixel_24[1];
			rgb[out++] = pixel_24[2];

			//Convert formats and push into chars. 
			pixel32 = Camera::convertPixelYUYV422toRGB8(y4, u, v);
			pixel_24[0] = (pixel32 & 0x000000ff);
			pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
			pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

			//Push out. 
			rgb[out++] = pixel_24[0];
			rgb[out++] = pixel_24[1];
			rgb[out++] = pixel_24[2];


	
		}
	}



	void Camera::convertImageYUYV422toRGB8(const unsigned char *yuv, unsigned char *rgb,
			const unsigned int width, const unsigned int height)
	{
		unsigned int in = 0;
		unsigned int out = 0;
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
		unsigned int in = 0;
		unsigned int out = 0;
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
		unsigned int in = 0;
		unsigned int out = 0;
		for (out = 0; out < width*height; ++out)
		{
			mono[out] = (unsigned char) (rgb[in]*0.3 + rgb[in+1]*0.59 + rgb[in+2]*0.11);
			in+=3;
		}
	}

	void Camera::setConfiguration(const Configuration &c)
	{
		assert ( (c.format == FORMAT7 && c.format7.xMax > 0 && c.format7.yMax > 0 && c.format7.xOffset >= 0 && c.format7.yOffset >= 0 ) ||
			 ( c.format != FORMAT7 && c.width > 0 && c.height > 0));

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
			if( partialConfig.format == FORMAT7 ){
			    if(partialConfig.format7.format != ANY && (*it).format7.format != partialConfig.format7.format)
				continue;
			    if (partialConfig.format7.xMax != 0 && (*it).format7.xMax > partialConfig.format7.xMax)
				continue;
			    if (partialConfig.format7.yMax != 0 && (*it).format7.yMax > partialConfig.format7.yMax)
				continue;

			    return *it;
			} else {
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

						case YUYV411:
							convertImageYUYV411toRGB8( currentFrame, buffer, width, height);
							return buffer;

#if ENABLE_VIDEO
						case MJPEG:{
						    // Init the video decoder on the first MJPEG decoding frame
						    if( this->internal == NULL){
							this->internal = new Priv;
							this->internal->decoder = new VideoDecoder(width, height,VideoDecoder::MJPEG, false );
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
				{
				    switch( this->activeConfiguration.format ){
					case RGB8:
					    convertImageRGB8toMONO8(currentFrame,buffer,width,height);
					    return buffer;

					case YUYV422:
						{
						//create a temporary buffer
						unsigned char *temp = new unsigned char[getFormatBufferSize(RGB8)];
						convertImageYUYV422toRGB8(currentFrame, temp, width, height);
						convertImageRGB8toMONO8(temp, buffer, width, height);
						delete [] temp;
						return buffer;
						}

					case MJPEG:
						{
#if ENABLE_VIDEO
						VideoDecoder dec(width, height,VideoDecoder::MJPEG, false );
						dec.nextFrame(currentFrame, this->getFormatBufferSize());
						convertImageRGB8toMONO8(dec.getFrame(), buffer, width, height);
						return buffer;
#endif
						}

					default:
					    ;
				    }
				    goto NOTIMP;
				}
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
		// Handle the case where this method is called before any other
		// method that actually performs a capture. If the currentFrame
		// is not set then most the conversions below will fail
		if(this->currentFrame == NULL ){
		    buffer = NULL;
		    return;
		}

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
						case YUYV411:
							convertImageYUYV411toRGB8( currentFrame, buffer, width, height);
							return;

#if ENABLE_VIDEO
						case MJPEG:{
						    // Init the video decoder on the first MJPEG decoding frame
							VideoDecoder decoder(width, height,VideoDecoder::MJPEG, false );
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
#if ENABLE_VIDEO
						VideoDecoder dec(width, height,VideoDecoder::MJPEG, false );
						dec.nextFrame(currentFrame, this->getFormatBufferSize());
						convertImageRGB8toMONO8(dec.getFrame(), buffer, width, height);
						return;
#endif
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
			case RAW8:
				return 3;
			case RAW16:
				return 6;
			default:
				return 0;
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
			if( this->conversionBuffer->length >= buffersize )
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

	void Camera::printFormat7(const Format7 f,const bool inUse)
	{
	    if(inUse)
		wclclog << "FORMAT7 (" << this->imageFormatToString(f.format) << ") "
		    << " Region: (" << f.xOffset << "," << f.yOffset << ")"
		    << " -> (" << f.xMax << "," << f.yMax << ")";
	    else
		wclclog << "FORMAT7 (" << this->imageFormatToString(f.format) << ") "
		    << " Region Available: (" << f.xOffset << "," << f.yOffset << ")"
		    << " -> (" << f.xMax << "," << f.yMax << ")"
		    << " OffsetStepSize: (" << f.xOffsetStepSize << "," << f.yOffsetStepSize << ")"
		    << " ExtentStepSize: (" << f.xStepSize << "," << f.yStepSize << ")";
	}

	void Camera::printDetails(bool state)
	{

	    Configuration a = this->getActiveConfiguration();
	    wclclog << "Camera ID:" << this->id << " (" << this->getTypeIdentifier() << ") |";
	    if( a.format == FORMAT7)
		this->printFormat7(a.format7,true);
	    else
		wclclog<< this->imageFormatToString(a.format) << ":" << a.width << "x" << a.height << "@" << a.fps;

	    wclclog << endl;

	    if ( state ){
			sort(this->supportedConfigurations.begin(),
				 this->supportedConfigurations.end(), configurationSort);

			wclcout << "Features/Modes" << endl;
			for(std::vector<Configuration>::iterator it =
				supportedConfigurations.begin(); it !=
				supportedConfigurations.end(); ++it ){
				Configuration c = *it;

				if(c.format == FORMAT7 ){
					wclclog << "\t";
					this->printFormat7(c.format7,false);
					wclclog << endl;
				} else {
					if( it == supportedConfigurations.begin()){
						wclclog << "\t" << this->imageFormatToString(c.format) << " " << c.width << "x" << c.height << " @" << c.fps;
					}
					else {
						--it;
						Configuration prev = *it;
						++it;
						if( c.format == prev.format && c.width == prev.width && c.height == prev.height )
							wclclog << "," << c.fps;
						else {
							wclclog << endl;
							wclclog << "\t" << this->imageFormatToString(c.format) << " "
								<< c.width << "x" << c.height << " @" << c.fps;
						}
					}
				}
			}
			wclclog << endl;
	    }
	}

	const char *Camera::imageFormatToString(const ImageFormat f )
	{
	    switch(f)
	    {
		case MJPEG: return "MJPEG";
		case YUYV411: return "YUYV411";
		case YUYV422: return "YUYV422";
		case YUYV444: return "YUYV444";
		case RGB8: return "RGB8";
		case RGB16: return "RGB16";
		case RGB32: return "RGB32";
		case BGR8: return "BGR8";
		case MONO8: return "MONO8";
		case MONO16: return "MONO16";
		case RAW8: return "RAW8";
		case RAW16: return "RAW16";
		case FORMAT7: return "FORMAT7";
		case ANY:
		default:
		    return "UNKNOWN:";
	    };
	}

}


