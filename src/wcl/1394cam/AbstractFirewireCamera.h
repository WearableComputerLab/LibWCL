#ifndef _ABSTRACTFIREWIRECAMERA_H_
#define _ABSTRACTFIREWIRECAMERA_H_

// an abstract class definition of other classes to implement.
class AbstractFirewireCamera
{
	public:
		AbstractFirewireCamera();
		virtual unsigned char* getFrame() = 0;
		virtual ~AbstractFirewireCamera();
};

#endif // _ABSTRACTFIREWIRECAMERA_H_
