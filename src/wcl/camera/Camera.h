/*-
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
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

#ifndef WCL_CAMERA_CAMERA_H
#define WCL_CAMERA_CAMERA_H

#include <wcl/maths/SMatrix.h>
#include <string>

namespace wcl
{

	//struct for information about the image buffers we create
	struct CameraBuffer {
		void* start;
		size_t length;

                CameraBuffer();
	};

	/**
         * An abstract base class representing a camera
	 */
	class Camera
	{

		public:
			typedef std::string CameraID;

			struct Distortion {
			    SMatrix cameraToWorld;
			    float factor[4];

			    Distortion():
				cameraToWorld(4)
			    {
				factor[0]=factor[1]=factor[2]=factor[3]=0.0;
			    }
			};

			/**
			 * Enumeration of supported image formats.
			 */
			enum ImageFormat
			{
				MJPEG, //Motion JPEG Format.
				YUYV422, // YUYV 4:2:2 format.
				YUYV411, // YUYV 4:2:2 format.
				RGB8,
                                RGB16,
				RGB32,
                                BGR8,
				MONO8,
				MONO16
				/*
				   FORMAT7,
				   BAYER,
				   EXIF
				*/

			};


			/**
			 * Camera exposure modes.
			 */
			enum ExposureMode
			{
				/**
				 * Auto exposure
				 */
				AUTO,

				/**
				 * Manual exposure
				 */
				MANUAL,

				/**
				 * Auto exposure with shutter speed priority.
				 */
				AUTO_SHUTTER_PRIORITY,

				/**
				 * Auto exposure with aperture priority.
				 * Note that most webcams that don't have adjustable
				 * apertures only support this mode or manual.
				 */
				AUTO_APERTURE_PRIORITY
			};

			/**
			 * Control names for controlling various parameters of the camera.
			 */
			enum Control
			{
				APERTURE,
				BRIGHTNESS,
				CONTRAST,
				SATURATION,
				GAIN,
				POWER_FREQUENCY,
				WHITE_BALANCE,
				SHARPNESS,
				EXPOSURE,
				FRAMERATE,
				ISO

			};

                        virtual ~Camera();

			/**
			 * Prints useful information about the camera to the console.
			 *
			 *  o Supported image modes
			 *  o Supported resolutions and framerates
			 *  o Available controls
			 *
			 */
			virtual void printDetails() = 0;

			/**
			 * Obtain the unique identify for this camera
			 */
			virtual CameraID getID() const { return this->id; }

			/**
			 * Sets the image format of the camera.
			 *
			 * @param f The data format for the images.
			 * @param width The width of the image in pixels
			 * @param height The height of the image in pixels
			 * @throws Exception if the format can't be set
			 */
			virtual void setFormat(const ImageFormat f, const unsigned width, const unsigned height);
                        ImageFormat getImageFormat() const { return this->format; }

			///XXX NOT YET - benjsc 20100211 std::vector<ImageFormat>getSupportedFormats() const;
			
                        unsigned getFormatWidth() const { return this->width; }
                        unsigned getFormatHeight() const { return this->height; }
			unsigned getFormatFPS() const { return this->fps; };

			/**
			 * Sets the exposure mode of the camera.
			 *
			 * @param t The exposure mode to change to.
			 * @throws Exception if the format can't be set
			 */
			virtual void setExposureMode(const ExposureMode t) = 0;

                        /**
                         * Sets the control value for the camera
                         *
                         * TODO This will need more work in the future as some
                         * features can't be set via a single int
			 *
			 * @param control The control to set
			 * @param value The value to set that control to
			 * @throws Exception if the control fails to set
                         */
			virtual void setControlValue(const Control control, const int value) = 0;

			/**
			 * Returns an image buffer for use in a program.
			 *
			 * @return a char array containing the image buffer.
			 */
			virtual const unsigned char* getFrame() = 0;

			/**
			 * Start the camera capturing
			 */
			virtual void startup() = 0;

			/**
			 * Closes the connection to the camera, freeing inuse * buffer memory
			 */
			virtual void shutdown() = 0;

			/**
			 * Indicates if there is a known undistortion matrix for
			 * the camera
			 */
			virtual bool hasDistortionMatrix() const { return false; }

			/**
		         * Obtain the distortion Matrix for the camera
			 */
			virtual Distortion getDistortion() const;


			// Helper routines

			/**
			 * Converts a single pixel from yuv422 to rgb8
			 */
			static int convertPixelYUV422toRGB8(const int y, const int u, const int v);

			/**
			 * Converts a YUYV422 buffer to an RGB8 buffer
			 */
			static void convertImageYUV422toRGB8(const unsigned char *yuv, unsigned char *rgb,
						    const unsigned int width, const unsigned int height);

			static void convertImageMONO8toRGB8(const unsigned char *mono8, unsigned char *rgb,
							    const unsigned int width, const unsigned int height );
                protected:

                        Camera();

                        void allocateBuffers(const size_t size, const unsigned count);
                        void destroyBuffers();

			/**
			 * Array of buffers we are using.
			 */
			CameraBuffer* buffers;

			int bufferSize;

			/**
			 * The number of buffers we have
			 */
			int numBuffers;

                        /**
                         * The Current Camera related parameters
                         */
                        ImageFormat format;
                        unsigned height;
                        unsigned width;
			unsigned fps;

			/**
		         * Default Distortion parameters
			 */
			Distortion distortion;

			/**
			 * The unique ID for this camera
			 */
			CameraID id;
	};

};

#endif

