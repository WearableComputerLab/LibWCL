#ifndef _OSXIEEE1394CAM_H_
#define _OSXIEEE1394CAM_H_	

#include "../AbstractIEEE1394Cam.h"
#include "Camera.h"
#include "../debug.h"

class OSXIEEE1394Cam : public AbstractIEEE1394Cam
{
	public:
		// default constructor assume image size of 640x480
		// and image type of RGB
		OSXIEEE1394Cam();

		// constructor that enables the user to enter the 
		// width and height of the image. The default colour
		// space is RGB
		OSXIEEE1394Cam( int width, int height );
		
		unsigned char* getFrame();
		~OSXIEEE1394Cam(); 
	private:
		cfox::Camera* camera;
		unsigned char* image_buffer;

};

#endif // _OSXIEEE1394CAM_H_
