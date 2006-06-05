#ifndef _LINUXIEEE1394CAM_H_
#define _LINUXIEEE1394CAM_H_	

#include "../AbstractIEEE1394Cam.h"
#include "video.h"
#include "../debug.h"

class LinuxIEEE1394Cam : public AbstractIEEE1394Cam
{
	public:
		LinuxIEEE1394Cam();
		unsigned char* getFrame();
		~LinuxIEEE1394Cam(); 
	private:
		AR2VideoParamT* artk_params;
		unsigned char* image_buffer;
};

#endif // _LINUXIEEE1394CAM_H_
