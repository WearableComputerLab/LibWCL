#ifndef _LINUXIEEE1394CAM_H_
#define _LINUXIEEE1394CAM_H_	

#include "../AbstractIEEE1394Cam.h"
#include "../debug.h"
#include "firewire.h"

class LinuxIEEE1394Cam : public AbstractIEEE1394Cam
{
	public:
		LinuxIEEE1394Cam();
		unsigned char* getFrame();
		~LinuxIEEE1394Cam(); 
	private:
		unsigned char* image_buffer;
		VideoParams* handle;
		int xsize;
		int ysize;
};

#endif // _LINUXIEEE1394CAM_H_
