#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "firewire.h"

class Camera {

	protected:

		int xsize;
		int ysize;

		VideoParams* handle;

		Camera();

	public:

		/* Constructor initialises the connection to the camera */
		Camera(char* mode, float rate, int card, int node);
		virtual ~Camera();

		/* Start capture needs to be called to initialise the streaming
		 * from the camera */
		virtual void startCapture();

		/* GetImage will pull an image from the camera which should be 
		 * in the size defined by getXSize() and getYSize(); */
		virtual char* getCameraImage();

		/* The X and Y dimensions of the camera image */
		int getXSize();
		int getYSize();
};

#endif
