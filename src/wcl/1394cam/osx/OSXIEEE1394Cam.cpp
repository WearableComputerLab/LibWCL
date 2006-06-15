#include "OSXIEEE1394Cam.h"

// constructor
OSXIEEE1394Cam::OSXIEEE1394Cam()
{
	try
	{
       // create the camera object
//        this->camera = new cfox::Camera( cfox::Mode_640x480_Mono8, cfox::FPS_30, 0, cfox::BasicCamera );
        this->camera = new cfox::Camera( cfox::Mode_640x480_RGB, cfox::FPS_30, 0, cfox::BasicCamera );

        // check that memory was allocated
        if( this->camera == NULL )
        {
                gen_fatal( "memory couldn't be obtained for the camera object" );
        }

        // initialize the camera caputuring stuff.
        IOReturn result = this->camera->start();

        // check that the camera could be initialized okay.
        if( result != noErr )
        {
                gen_fatal( "Camera could not be started, is it plugged in?" );
        }
	}
	catch( ... )
	{
		gen_fatal( "caught an exception" );
	}
}

// constructor that enables the user to enter the 
// width and height of the image. The default colour
// space is RGB
OSXIEEE1394Cam::OSXIEEE1394Cam( int width, int height )
{
	message( "not implemented yet" );
}

// returns a frame from the camera.
unsigned char* OSXIEEE1394Cam::getFrame()
{
	try
	{
        // don't know what this does but it was in the example code.
        CFRunLoopRunInMode( kCFRunLoopDefaultMode, 1, true );
        
        // return a frame from the camera.
        return this->camera->getFrame( cfox::MODE_FRAME, 0, 0 );
	}
	catch( ... )
	{
		gen_fatal( "caught an exception" );
	}
	return NULL;
}

// destructor, doesn't do anything, i'm sure there is stuff that it can do
// though
OSXIEEE1394Cam::~OSXIEEE1394Cam()
{
}
