#include "OSXIEEE1394Cam.h"

// constructor
OSXIEEE1394Cam::OSXIEEE1394Cam()
{
       // create the camera object
        //this->camera = new cfox::Camera( cfox::Mode_640x480_Mono8, cfox::FPS_30, 0, cfox::BasicCamera );
        this->camera = new cfox::Camera( cfox::Mode_160x120_YUV444, cfox::FPS_30, 0, cfox::BasicCamera );

        // check that memory was allocated
        if( this->camera == NULL )
        {
                gen_fatal( "memory couldn't be obtained for the camera object" );
        }

	this->initialize();
}

// constructor that enables the user to enter the 
// width and height of the image. The default colour
// space is mono
OSXIEEE1394Cam::OSXIEEE1394Cam( int width, int height )
{
	message( "not implemented yet" );
/*	
	// do a check on the width and height to make sure that we support it, 
	// I am only checking for 320x240 and 640x480 at the moment because it
	// the only sizes that I will use.
	if( width == 640 && height == 480 )
	{
		// create a new camera object with the specified image 
		// dimensions
		this->camera = new cfox::Camera( cfox::Mode_640x480_Mono8, cfox::FPS_30, 0, cfox::BasicCamera );

		// attempt to initialize the new camera object.
		this->initialize();
	}
	else if( width == 320 && height == 240 )
	{
		// create a new camera object with the specified image 
		// dimensions
		this->camera = new cfox::Camera( cfox::Mode_320x240_Mono8, cfox::FPS_30, 0, cfox::BasicCamera );

		// attempt to initialize the new camera object.
		this->initialize();
	}
	else
	{
		gen_fatal( "image dimension %dx%d not supported, currently only support image sizes are 320x240 and 640x480" );
	}
*/
}

// code that is common to a couple of the constructors, it just calls that 
// cameras start method and checks that it worked okay.
void OSXIEEE1394Cam::initialize()
{
	if( this->camera != NULL )
	{
		// initialize the camera caputuring stuff.
		IOReturn result = this->camera->start();

		// check that the camera could be initialized okay.
		if( result != noErr )
		{
			gen_fatal( "Camera could not be started, is it plugged in?" );
		}
	}
	else
	{
		gen_fatal( "the camera object has not yet been created, it is possible that this method is being called out of turn" );
	}

}

// returns a frame from the camera.
unsigned char* OSXIEEE1394Cam::getFrame()
{
	// don't know what this does but it was in the example code.
	CFRunLoopRunInMode( kCFRunLoopDefaultMode, 1, true );

	// return a frame from the camera.
	return this->camera->getFrame( cfox::MODE_FRAME, 0, 0 );
}

// destructor, doesn't do anything, i'm sure there is stuff that it can do
// though
OSXIEEE1394Cam::~OSXIEEE1394Cam()
{
}
