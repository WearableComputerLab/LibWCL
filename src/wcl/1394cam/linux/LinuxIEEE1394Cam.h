#ifndef _LINUXIEEE1394CAM_H_
#define _LINUXIEEE1394CAM_H_	

#include "../AbstractIEEE1394Cam.h"
#include "../debug.h"
#include "firewire.h"

class LinuxIEEE1394Cam : public AbstractIEEE1394Cam
{
	public:
		LinuxIEEE1394Cam();

		// constructor that takes the desired width and height of
		// the camera frame. Default is to return and RGB image.
		LinuxIEEE1394Cam( int width, int height );
		unsigned char* getFrame();
		~LinuxIEEE1394Cam(); 
	private:
		unsigned char* image_buffer;
		VideoParams* handle;
		int xsize;
		int ysize;
};

#endif // _LINUXIEEE1394CAM_H_
