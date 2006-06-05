#ifndef _1394CAM_H_
#define _1394CAM_H_

class 1394cam
{
public:
	// default constructor
	1394cam();

	// constructor that takes a char* argument of configuration options
	1394cam( char* );

	// constructor that takes a struct argument that contains the 
	// configuration options.
//	1394cam( some kind of struct );

	// method to retrieve an image from the camera.
	unsigned char* getImage();

	// destructor
	~1394cam();
private:
	// a pointer to the memory location of the image from the camera
	unsigned char* imageBuffer;
};

#endif //_1394CAM_H_
