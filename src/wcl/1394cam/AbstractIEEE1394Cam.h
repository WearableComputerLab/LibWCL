#ifndef _ABSTRACTIEEE1394CAM_H_
#define _ABSTRACTIEEE1394CAM_H_

// an abstract class definition of other classes to implement.
class AbstractIEEE1394Cam
{
	public:
		AbstractIEEE1394Cam();
		virtual unsigned char* getFrame() = 0;
		virtual ~AbstractIEEE1394Cam();
};

#endif // _ABSTRACTIEEE1394CAM_H_
