#ifndef _IEEE1394CAM_H_
#define _IEEE1394CAM_H_

#include "AbstractIEEE1394Cam.h"
#include "debug.h"

class IEEE1394Cam
{
public:
	// default constructor
	IEEE1394Cam();
	
	// constructor that enables the user to pass in a width and height
	// value. The default is to return an RGB image.
	IEEE1394Cam( int width, int height );

	// constructor that takes a char* argument of configuration options
	IEEE1394Cam( char* );

	// constructor that takes a struct argument that contains the 
	// configuration options.
//	1394cam( some kind of struct );

	// method to retrieve an image from the camera.
	unsigned char* getFrame();

	// destructor
	~IEEE1394Cam();
private:
	// a pointer to the camera
	AbstractIEEE1394Cam *camera;

	// a pointer to the memory location of the image from the camera
	unsigned char* imageBuffer;
};

#endif //_IEEE1394CAM_H_
