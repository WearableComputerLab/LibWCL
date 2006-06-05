#include "../debug.h"
#include "camera.h"

#include <unistd.h>

#include <firewire/firewire.h>

Camera::Camera(){
}

Camera::Camera(char* mode, float rate, int card, int node){
	/* The default mode shoudl probably be something like: 
	 * ("640x480_YUV411", 15, -1, -1) */

	char* rateStr;
	if(rate == 30) rateStr = "30";
	else if(rate == 15) rateStr = "15";
	else if(rate == 7.5) rateStr = "7.5";
	else if(rate == 3.75) rateStr = "3.75";
	else if(rate == 1.875) rateStr = "1.875";
	else fatal("You must specify a valid video framerate [30, 15, 7.5, 3.75, 1.875]. You specified %f", rate);

	char cameraConf[1024]; //= "-mode=320x240_YUV422 -rate=7.5 -card=0 -node=2";
	snprintf(cameraConf, 1024, "-mode=%s -rate=%s -card=%d -node=%d", mode, rateStr, card, node);

	/* Display the possible video configuation options so I dont 
	 * have to keep looking in artoolkit/lib/SRC/VideoLinus1394/
	 */
	//videoDisplayOptions();

	/* Open the 1394 video camera */
	handle = videoOpenHandle(cameraConf);
	if(handle == NULL)
		fatal("Error opening the video camera with options: %s", cameraConf);

	error("Camera stream opened succesfully: %s", cameraConf);


	/* Find the resoltion of the video stream we opened */
	if(videoGetSize(&xsize, &ysize, handle) < 0)
		fatal("Error finding size of the video image");

	error("Camera image size %d x %d", xsize, ysize);


}


/* TODO write a proper destructor */
Camera::~Camera(){


}

void Camera::startCapture(){

	videoStartCapture(handle);

}

char* Camera::getCameraImage(){
	uint8_t *cameraImage = NULL;
	
	/* Tell ART to begin capture on the next camera image */
	videoCaptureNext(handle);

	while(true){
		/* Get the captured image from ART */
		cameraImage = (uint8_t*) videoGetImage(handle);
		/* If an image is not ready yet then we wait a while until it is (30/sec) */
		/* TODO Should this really wait until new images are ready ?   Im sure returning
		 * some kind of error and letting the client application be able to use the spare 
		 * CPU cycles would be kind of better */
		if(cameraImage==NULL){
		   	usleep(100); 
		} else {
			return (char*)cameraImage;
		}
	}

}


int Camera::getXSize(){

	return xsize;

}
int Camera::getYSize(){

	return ysize;

}
