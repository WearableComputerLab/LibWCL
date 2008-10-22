#ifndef _DC1394CAMERA_H_
#define _DC1394CAMERA_H_

#include <dc1394/control.h>
#include <dc1394/conversions.h>

class DC1394
{
public:
	// default constructor
	DC1394();


	void startCapture();

	// method to get a frame from the camera
	unsigned char* getFrame( void );

	// method to shut down the camera.
	void close_camera( void );

	// method to change to the video mode specified by the string.
	void setVideoMode( char* newVideoMode );

	// method to change to the framerate specified by the string.
	void setFramerate( char* newFramerate );

	// method to change to the ISO speed
	void setISOSpeed( char* newISOSpeed );

	// method to set exposure
	void setExposure( char* exposure );

	void setShutter( char* shutter );
	// method to set brightness
	void setBrightness( char* brightness );

	// method to set the gain
	void setGain( char* gain );

	// method to display to the user the different video modes available
	void printSupportedVideoModes();

	// method to display to the user the different framerates available
	void printSupportedFramerates();

	// display a bunch of info about what the camera is capable of
	void printFeatureSet();

	// tell the camera that it is to work in trigger mode
	void useTrigger( bool turnOn );

	// returns the width of the image we are working with
	int getWidth();

	// returns the height of the image we are working with
	int getHeight();

	// set the flag that tells us if we need to convert from a bayer image or not
	void setUseBayer( bool useBayer );

	// return whether or not we need to convert from bayer or not
	bool getUseBayer();

	// find out what sort of conversion method to use.
	void setBayerMethod( char* bayerMethod );

	// find out what sort of bayer color filter to use
	void setColorFilter( char* colorFilter );

	// deconstructor
	~DC1394();
private:
	void cleanup( void );

	// method to populate videoModes with data
	void getSupportedVideoModes();

	// method to populate framerates with data
	void getSupportedFramerates();

	// method to check to see if a video mode is supported
	bool isVideoModeSupported( dc1394video_mode_t videoMode );

	// method to check to see if a given framerate is supported
	bool isFramerateSupported( dc1394framerate_t framerate );

	// configuration method that reads in values from a config file.
	//void configure();

	// fetch the image dimensions based on the camera setup
	void fetchImageProperties();
private:
	// the camera that we are interested in.
	dc1394camera_t* camera;

	// an array of all the cameras that are attatched to the system.
	dc1394camera_t **cameras;

	// the number of cameras that are attached to the system
	uint32_t numCameras;

	// a frame returned from the dc1394 capture device
	dc1394video_frame_t* frame;
	dc1394video_mode_t videoMode;
	dc1394video_modes_t videoModes;
	dc1394framerates_t framerates;
	dc1394framerate_t framerate;

	// some memory to hold the error code that is returned from the functions.
	dc1394error_t result;

	unsigned char* convertedImage;

	// boolean flag that says whether or not we are converting from bayer or not
	bool useBayer;

	// if we are converting from bayer then use this filter
	dc1394color_filter_t colorFilter;

	// if we are converting from bayer then use this method
	dc1394bayer_method_t bayerMethod;

	// the number of pixels wide the image is
	uint32_t width;

	// the number of pixels high the image is
	uint32_t height;

	// the depth of the image
	uint32_t depth;
};
#endif // _DC1394CAMERA_H_
