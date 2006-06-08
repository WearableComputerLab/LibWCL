#include "IEEE1394Cam.h"

#ifdef MACOSX
#include "osx/OSXIEEE1394Cam.h"
#elif defined(LINUX)
#include "linux/LinuxIEEE1394Cam.h"
#endif

// constructor
IEEE1394Cam::IEEE1394Cam()
{
// check if we are on a mac
#ifdef MACOSX
	message( "attempting to create an OSXIEEE1394Cam" );
	this->camera = new OSXIEEE1394Cam();
	message( "finished creating an OSXIEEE1394Cam" );
#elif defined(LINUX)
	message( "attempting to create a LinuxIEEE1394Cam" );
	this->camera = new LinuxIEEE1394Cam();	
	message( "finished creating a LinuxIEEE1394Cam" );
#endif
}

// constructor
IEEE1394Cam::IEEE1394Cam( int width, int height )
{
// check if we are on a mac
#ifdef MACOSX
	message( "attempting to create an OSXIEEE1394Cam" );
	this->camera = new OSXIEEE1394Cam( width, height );
	message( "finished creating an OSXIEEE1394Cam" );
#elif defined(LINUX)
	message( "attempting to create a LinuxIEEE1394Cam" );
	this->camera = new LinuxIEEE1394Cam( width, height );	
	message( "finished creating a LinuxIEEE1394Cam" );
#endif
}


// method to retrieve an image from the camera.
unsigned char* IEEE1394Cam::getFrame()
{
	return this->camera->getFrame();
}

// destructor
IEEE1394Cam::~IEEE1394Cam()
{
	// do a tidy up.
	delete this->camera;
}
