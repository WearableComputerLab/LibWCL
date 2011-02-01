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

#include <string>
#include <vector>
#include <wcl/api.h>
#include <wcl/maths/SMatrix.h>

namespace wcl
{

	/**
	 * An abstract base class representing a camera
	 */
	class WCL_API Camera
	{

		public:
			typedef std::string CameraID;

			struct CameraParameters {
				SMatrix intrinsicMatrix;
				float distortion[4];

				/**
				 * A useful constructor.
				 *
				 * @param focalLengthX The horizontal focal length, in pixels.
				 * @param focalLengthY The vertical focal length, in pixels.
				 * @param principlePointX The principle point (true centrepoint of the lens)
				 * @param principlePointY The principle point (true centrepoint of the lens)
				 * @param k1 Radial distortion coefficient 1.
				 * @param k2 Radial distortion coefficient 2.
				 * @param p1 Tangential distortion coefficient 1.
				 * @param p2 Tangential distortion coefficient 2.
				 */
				CameraParameters(float focalLengthX,
						float focalLengthY,
						float principalPointX,
						float principalPointY,
						float k1,
						float k2,
						float p1,
						float p2) 
					: intrinsicMatrix(4)
				{
					/**
					 * Generate an intrinsic matrix. For mor information
					 * on how this is done, see the following URL:
					 *
					 * http://www.vision.caltech.edu/bouguetj/calib_doc/htmls/parameters.html
					 */
					intrinsicMatrix[0][0]=focalLengthX;
					intrinsicMatrix[0][1]=0.;
					intrinsicMatrix[0][2]=principalPointX;
					intrinsicMatrix[0][3]=0.;
					intrinsicMatrix[1][0]=0.;
					intrinsicMatrix[1][1]=focalLengthY;
					intrinsicMatrix[1][2]=principalPointY;
					intrinsicMatrix[1][3]=0;
					intrinsicMatrix[2][0]=0.;
					intrinsicMatrix[2][1]=0.;
					intrinsicMatrix[2][2]=1.;
					intrinsicMatrix[2][3]=0.;


					// set the distortion array. easy
					distortion[0] = k1;
					distortion[1] = k2;
					distortion[2] = p1;
					distortion[3] = p2;
				}

				CameraParameters():
					intrinsicMatrix(4)
				{
					distortion[0]=distortion[1]=distortion[2]=distortion[3]=0.0f;
				}
			};

			/**
			 * Enumeration of supported image formats.
			 */
			enum ImageFormat
			{
				ANY,
				MJPEG, //Motion JPEG Format.
				YUYV422, // YUYV 4:2:2 format.
				YUYV411, // YUYV 4:1:1 format.
				RGB8, //  8 8 8
				RGB16, // 16 16 16
				RGB32, // 32 32 32
				BGR8, // 8 8 8
				MONO8, //greyscale 8bpp
				MONO16 // greyscale 16bpp
					/*
					   FORMAT7,
					   BAYER,
					   EXIF
					   */

			};

			struct Configuration
			{
				float fps;
				unsigned width;
				unsigned height;
				ImageFormat format;

				Configuration()
					: fps(0), width(0), height(0), format(ANY) {}
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
			 * @param full If true display full details about the camera else id type and current mode
			 */
			virtual void printDetails(bool full = true);

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
			virtual void setConfiguration(const Configuration &c) = 0;
			Configuration getActiveConfiguration() const { return this->activeConfiguration; }

			unsigned getFormatBytesPerPixel() const;
			unsigned getFormatBytesPerPixel(const ImageFormat ) const;
			unsigned getFormatBufferSize() const;
			unsigned getFormatBufferSize(const ImageFormat) const;

			std::vector<Configuration> getSupportedConfigurations() const;

			Configuration findConfiguration(Configuration partialConfig) const;

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
			 * Returns the value of a control for the camera.
			 * 
			 * @param control The control to receive.
			 */
			virtual int getControlValue(const Control control) = 0;

			/**
			 * Gets the next frame from the camera, stores it internally.
			 */
			virtual void update() = 0;

			/**
			 * Returns an image buffer for use in a program.
			 * Blocking function, calls update internally to get 
			 * the next frame from the hardware.
			 *
			 * @return an unsigned char array containing the image buffer.
			 */
			virtual const unsigned char* getFrame();

			/**
			 * Return an image buffer in the specified format. The
			 * camera format is unchanged, and software is used
			 * to convert the format to the requested format
			 *
			 * This calls update() to get the next frame from the hardware.
			 *
			 * @param f The format the frame should be returned in
			 * @return an unsigned char array in the requested format (Note the datasize of this frame may be larger)
			 */
			virtual const unsigned char *getFrame(const ImageFormat f);

			/**
			 * Gets the current frame from the camera.
			 * This method does not call update(), so successive
			 * calls will return the same image.
			 */
			virtual const unsigned char* getCurrentFrame() const;

			/**
			 * Gets the current image from the camera in the specified format.
			 *
			 * This takes a buffer as an argument, and places a copy of the 
			 * image into that buffer, in the required format.
			 *
			 * This is the only way you can get images in different formats 
			 * with a const camera. This is because successive calls to getCurrentFrame
			 * would modify the internal conversion buffer. This is bad.
			 *
			 * @param buffer The buffer to place the image into. This should be the correct
			 *               size for the ImageFormat requested. 
			 *
			 * @param format The desired image format.
			 */
			virtual void getCurrentFrame(unsigned char* buffer, const ImageFormat& format) const;


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
			virtual bool hasParameters() const;

			/**
			 * Obtain the distortion Matrix for the camera
			 */
			virtual CameraParameters getParameters() const;
			virtual void setParameters(const CameraParameters& p);


			// Helper routines

			/**
			 * Converts a single pixel from yuv422 to rgb8
			 */
			static int convertPixelYUYV422toRGB8(const int y, const int u, const int v);

			/**
			 * Converts a YUYV422 buffer to an RGB8 buffer
			 */
			static void convertImageYUYV422toRGB8(const unsigned char *yuv, unsigned char *rgb,
					const unsigned int width, const unsigned int height);
			static void convertImageYUYV411toRGB8(const unsigned char *yuv, unsigned char *rgb,
					const unsigned int width, const unsigned int height);

			static void convertImageMONO8toRGB8(const unsigned char *mono8, unsigned char *rgb,
					const unsigned int width, const unsigned int height );

			static void convertImageRGB8toMONO8(const unsigned char* rgb, unsigned char* mono, const unsigned width, const unsigned height);
		protected:
			//struct for information about the image buffers we create
			struct CameraBuffer {
			    void* start;
			    size_t length;

			    CameraBuffer();
			};


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
			unsigned numBuffers;

			/**
			 * The Current Camera related parameters
			 */
			Configuration activeConfiguration;

			/**
			 * Default Distortion parameters
			 */
			CameraParameters parameters;
			bool areParametersSet;

			/**
			 * The unique ID for this camera
			 */
			CameraID id;

			std::vector<Configuration> supportedConfigurations;

			virtual const char *getTypeIdentifier() const = 0;

			unsigned char* currentFrame;
		private:
			CameraBuffer *conversionBuffer;

			// Forward declaration of internal camera struct;
			struct Priv;
			Priv *internal;

			void setupConversionBuffer( const size_t buffersize );
			const char *imageFormatToString( const ImageFormat );
	};

};

#endif

