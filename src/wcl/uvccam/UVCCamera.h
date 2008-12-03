/*-
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
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

#ifndef UVCCAMERA_H
#define UVCCAMERA_H

#include <linux/videodev2.h>
#include <string>

#define AUTO_EXPOSURE_CTRL 10094849

using namespace std;


namespace wcl
{

	//struct for information about the image buffers we create
	struct bufferT {
		void* start;
		size_t length;
	};

	/**
	 * A class for talking to USB cameras that follow the UVC standard.
	 * This might actually work for any camera that has a Video4Linux2
	 * driver, but untested.
	 */
	class UVCCamera
	{

		public:

			/**
			 * Enumeration of supported image formats.
			 */
			enum ImageFormat
			{
				/**
				 * Motion JPEG Format.
				 */
				MJPEG,

				/**
				 * YUYV 4:2:2 format.
				 */
				YUYV
			};

			/**
			 * Enumeration for supported read acess modes.
			 * Currently, only MMAP works. However, it is unlikely
			 * that the application programmer will ever need to set the read mode,
			 * it is handled automagically depending on what the camera supports.
			 */
			enum ReadMode
			{
				/**
				 * Camera supports using read()
				 */
				CALL_READ,

				/**
				 * Access using mmaped buffers.
				 */
				MMAP,

				/**
				 * Access using pointer swapping.
				 */
				POINTER
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
				BRIGHTNESS      = V4L2_CID_BRIGHTNESS,
				CONTRAST        = V4L2_CID_CONTRAST,
				SATURATION      = V4L2_CID_SATURATION,
				GAIN            = V4L2_CID_GAIN,
				POWER_FREQUENCY = V4L2_CID_POWER_LINE_FREQUENCY,
				WHITE_BALANCE   = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
				SHARPNESS       = V4L2_CID_SHARPNESS,
				EXPOSURE        = V4L2_CID_EXPOSURE
			};


			/**
			 * Opens a connection to a camera.
			 *
			 * @param filename The path to the camera device.
			 */
			UVCCamera(string filename = "/dev/video0");


			/**
			 * Prints useful information about the camera to the console.
			 *
			 *  o Supported image modes
			 *  o Supported resolutions and framerates
			 *  o Available controls
			 *
			 */
			void printDetails();


			/**
			 * Sets the image format of the camera.
			 *
			 * @param f The data format for the images.
			 * @param width The width of the image in pixels
			 * @param height The height of the image in pixels
			 */
			void setFormat(ImageFormat f, unsigned width, unsigned height);


			/**
			 * Sets the exposure mode of the camera.
			 *
			 * WARNING:
			 *
			 * Cameras seem to report all exposure modes, but do not actually
			 * support them all. So, if you get an error maybe try a different mode.
			 *
			 * TODO:
			 *
			 * The mode should be smart so if a particular auto isn't supported it
			 * tries a different one. This is not implemented yet.
			 *
			 * @param t The exposure mode to change to.
			 */
			bool setExposureMode(ExposureMode t);


			bool setControlValue(Control control, int value);

			/**
			 * Returns an image buffer for use in a program.
			 *
			 * DO NOT DELETE THIS BUFFER.
			 * It is managed by this class but mostly the driver.
			 * calling delete on this buffer should cause the program to crash.
			 *
			 * @return a char array containing the image buffer.
			 */
			unsigned char* getFrame();


			/**
			 * Returns the size of the image buffer, in bytes.
			 */
			int getBufferSize();


			/**
			 * Closes the connection to the camera and deletes any mmap'd buffers.
			 */
			void shutdown();

		private:
			/**
			 * File handle to the camera.
			 */
			int cam;

			/**
			 * Whether the camera is ready for capture or needs
			 * some more setup first. Used internally.
			 */
			bool isReadyForCapture;

			/**
			 * Loads the controls supported by the camera.
			 * At the momenet it just prints them out, it should be 
			 * storing them.
			 */
			void loadControls();


			/**
			 * Gets the capabilities of the camera.
			 */
			void loadCapabilities();


			/**
			 * Users shouldn't have to know the steps involved in preparing the
			 * camera for capture. They should just open camera, set image mode,
			 * get frame. However, we need to do more than that, so that is
			 * what this function is for.
			 */
			void prepareForCapture();

			/**
			 * The read mode we are using for the camera.
			 */
			ReadMode mode;

			/**
			 * Array of buffers we are using.
			 */
			bufferT* buffers;

			int bufferSize;

			/**
			 * The number of buffers we have
			 */
			int numBuffers;

	};

};

#endif

