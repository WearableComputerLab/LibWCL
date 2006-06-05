#include "IEEE1394Cam.h"

#ifdef MACOSX
#include "osx/OSXIEEE1394Cam.h"
#endif


// constructor
IEEE1394Cam::IEEE1394Cam()
{

// check if we are on a mac
#ifdef MACOSX
	this->camera = new OSXIEEE1394Cam();
	message( "creating an OSXIEEE1394Cam" );
#endif

	message( "in IEEE1394 constructor" );
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
