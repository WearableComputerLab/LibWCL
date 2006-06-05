#ifndef _OSXIEEE1394CAM_H_
#define _OSXIEEE1394CAM_H_	

#include "../AbstractIEEE1394Cam.h"
#include "video.h"
#include "../debug.h"

class OSXIEEE1394Cam : public AbstractIEEE1394Cam
{
	public:
		OSXIEEE1394Cam();
		unsigned char* getFrame();
		~OSXIEEE1394Cam(); 
	private:
		AR2VideoParamT* artk_params;
		unsigned char* image_buffer;

};

#endif // _OSXIEEE1394CAM_H_
