#include "OSXIEEE1394Cam.h"

// constructor
OSXIEEE1394Cam::OSXIEEE1394Cam()
{
	// the string that we are going to pass to artoolkit
	char init_string [] = "-nodialog -width=640 -height=480 -pixelformat=24 -fps";
	//char init_string [] = "-nodialog -width=320 -height=240 -pixelformat=24";

	// attempt to open the camera
	this->artk_params = ar2VideoOpen( init_string );

	// check to see if the camera was opened okay
	if( this->artk_params == NULL )
	{
		gen_fatal( "Couldn't open camera. Is it plugged in?" );
	}

	// start the capturing.
	int started = ar2VideoCapStart( this->artk_params );

	// check that capturing was started okay
	if( started == -1 )
	{
		gen_fatal( "camera capturing could not be started" );
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
	while( ( this->image_buffer = ar2VideoGetImage( this->artk_params ) ) == NULL )
		;
/*
	unsigned char*  temp = image_buffer;
	for( int i = 0; i < 640 * 480; i++ )
	{
		*temp = 255 - *temp;
		temp++;
	}
*/
	return this->image_buffer;
}

OSXIEEE1394Cam::~OSXIEEE1394Cam()
{
	// close the image grabber
	int stopped = ar2VideoCapStop( this->artk_params );

	// check that capturing was stopped okay
	if( stopped == -1 )
	{
		gen_fatal( "camera capturing could not be stopped cleanly" );
	}
}
