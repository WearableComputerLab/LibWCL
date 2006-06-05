#ifndef _IEEE1394CAM_H_
#define _IEEE1394CAM_H_

class IEEE1394Cam
{
public:
	// default constructor
	IEEE1394Cam();

	// constructor that takes a char* argument of configuration options
	IEEE1394Cam( char* );

	// constructor that takes a struct argument that contains the 
	// configuration options.
//	1394cam( some kind of struct );

	// method to retrieve an image from the camera.
	unsigned char* getImage();

	// destructor
	~IEEE1394Cam();
private:
	// a pointer to the memory location of the image from the camera
	unsigned char* imageBuffer;
};

#endif //_IEEE1394CAM_H_
