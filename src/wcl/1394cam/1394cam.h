#ifndef _1394CAM_H_
#define _1394CAM_H_

class 1394cam
{
public:
	// constructor
	1394cam();

	// method to retrieve an image from the camera.
	unsigned char* getImage();

	// destructor
	~1394cam();
private:
	// a pointer to the memory location of the image from the camera
	unsigned char* imageBuffer;
};
#endif //_1394CAM_H_
