/*
 *	Video capture subrutine for Linux/libdc1394 devices
 *	author: Kiyoshi Kiyokawa ( kiyo@crl.go.jp )
 *	        Hirokazu Kato ( kato@sys.im.hiroshima-cu.ac.jp )
 *
 *	Revision: 1.0   Date: 2002/01/01
 *
 */
/*
 *	Copyright (c) 2003-2005 Philip Lamb (PRL) phil@eden.net.nz. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *	1.1.0	2003-09-09	PRL		Based on Apple "Son of MungGrab" sample code for QuickTime 6.
 *								Added config option "-fps" to superimpose frame counter on video.
 *								Returns aligned data in ARGB pixel format.
 *  1.2.0   2004-04-28  PRL		Now one thread per video source. Versions of QuickTime
 *								prior to 6.4 are NOT thread safe, and with these earlier
 *								versions, QuickTime toolbox access will be serialised.
 *	1.2.1   2004-06-28  PRL		Support for 2vuy and yuvs pixel formats.
 *  1.3.0   2004-07-13  PRL		Code from Daniel Heckenberg to directly access vDig.
 *  1.3.1   2004-12-07  PRL		Added config option "-pixelformat=" to support pixel format
 *								specification at runtime, with default determined at compile time.
 *	1.4.0	2005-03-08	PRL		Video input settings now saved and restored.
 *  1.4.1   2005-03-15  PRL     QuickTime 6.4 or newer is now required by default. 
 *
 */
/*
 * 
 * This file is part of ARToolKit.
 * 
 * ARToolKit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * ARToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ARToolKit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
/*
 *  
 * The functions beginning with names "vdg" adapted with changes from
 * vdigGrab.c, part of the seeSaw project by Daniel Heckenberg.
 *
 * Created by Daniel Heckenberg.
 * Copyright (c) 2004 Daniel Heckenberg. All rights reserved.
 * (danielh.seeSaw<at>cse<dot>unsw<dot>edu<dot>au)
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the right to use, copy, modify, merge, publish, communicate, sublicence, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * TO THE EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NON-INFRINGEMENT.Ê IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

// ============================================================================
//	Private includes
// ============================================================================

#include "video.h"

// ============================================================================
//	Private definitions
// ============================================================================


#define AR_VIDEO_IDLE_INTERVAL_MILLISECONDS_MIN		20L
#define AR_VIDEO_IDLE_INTERVAL_MILLISECONDS_MAX		100L

#define AR_VIDEO_STATUS_BIT_READY   0x01			// Clear when no new frame is ready, set when a new frame is ready.
#define AR_VIDEO_STATUS_BIT_BUFFER  0x02			// Clear when buffer 1 is valid for writes, set when buffer 2 is valid for writes. 

// Early Mac OS X implementations of pthreads failed to define PTHREAD_CANCELED.
#ifdef PTHREAD_CANCELED
#  define AR_PTHREAD_CANCELLED PTHREAD_CANCELED
#else
#  define AR_PTHREAD_CANCELLED ((void *) 1);
#endif

AR2VideoParamT* gVid = NULL;
unsigned int gVidCount = 0;


AR2VideoParamT* ar2VideoOpen(char *config)
{
	static int		initF = 0;
	long			qtVersion = 0L;
	int			width = 0;
	int			height = 0;
	int			grabber = 1;
	int			showFPS = 0;
	int			showDialog = 1;
	OSErr			err_s = noErr;
	ComponentResult		err = noErr;
	int			err_i = 0;
	AR2VideoParamT		*vid = NULL;
	char* 			a, line[256];
	OSType			pixFormat = (OSType)0;
	long			bytesPerPixel;
	CGrafPtr		theSavedPort;
	GDHandle		theSavedDevice;
	Rect			sourceRect = {0, 0};

	message( "Input string: %s", config );

	// Process configuration options.
	a = config;
	if (a) {
		for(;;)
		{
			// skip over the whitspaces
			while( *a == ' ' || *a == '\t' )
			{
				 a++; // Skip whitespace.
			}
		
			// check if we have reached the end of the line.	
			if( *a == '\0' )
			{
				break;
			}

			// see if it is the width.
			if( strncmp( a, "-width=", 7 ) == 0 )
			{
				// grab a copy of the width
				sscanf( a, "%s", line );

				// check if the width was read in okay
				if( sscanf( &line[7], "%d", &width ) == 0 )
				{
					// dsiplay the option flags.
					ar2VideoDispOption();
					return NULL;
				}
				else
				{
					message( "width = %d", width );
				}
			}
			// see if it was the height flag
			else if( strncmp( a, "-height=", 8 ) == 0 )
			{
				sscanf( a, "%s", line );

				// check if the height was read in okay
				if (sscanf(&line[8], "%d", &height) == 0)
				{
					ar2VideoDispOption();
					return (NULL);
				}
				else
				{
					message( "height = %d", height );
				}
			}
			// see if the grabber was set.
			else if( strncmp( a, "-grabber=", 9 ) == 0 )
			{
				// copy the info into line
				sscanf( a, "%s", line );

				// try and rip out the grabber info
				if( sscanf( &line[9], "%d", &grabber ) == 0 )
				{
					ar2VideoDispOption();
					return (NULL);
				}
				else
				{
					message( "grabber = %d", grabber );
				}
			} 
			else if (strncmp(a, "-pixelformat=", 13) == 0) {
				sscanf(a, "%s", line);
				if (sscanf(&line[13], "%c%c%c%c", (char *)&pixFormat, ((char *)&pixFormat) + 1,
							((char *)&pixFormat) + 2, ((char *)&pixFormat) + 3) < 4) { // Try 4-cc first.
					if (sscanf(&line[13], "%li", (long *)&pixFormat) < 1) { // Fall back to integer.
						ar2VideoDispOption();
						return (NULL);
					}
				}
			}
			else if (strncmp(a, "-fps", 4) == 0)
			{
				showFPS = 1;
				
			} 
			else if (strncmp(a, "-nodialog", 9) == 0) {
				showDialog = 0;
			} else {
				ar2VideoDispOption();
				return (NULL);
			}

			while (*a != ' ' && *a != '\t' && *a != '\0') a++; // Skip non-whitespace.
		}
	}
	// If no pixel format was specified in command-line options,
	// assign the one specified at compile-time as the default.
	if (!pixFormat) {
		pixFormat = k24RGBPixelFormat;
	}

	switch (pixFormat) {
		case k2vuyPixelFormat:
		case kYUVSPixelFormat:
			bytesPerPixel = 2l;
			break; 
		case k24RGBPixelFormat:
		case k24BGRPixelFormat:
			bytesPerPixel = 3l;
			break;
		case k32ARGBPixelFormat:
		case k32BGRAPixelFormat:
		case k32ABGRPixelFormat:
		case k32RGBAPixelFormat:
			bytesPerPixel = 4l;
			break;
		default:
			fprintf(stderr, "ar2VideoOpen(): Unsupported pixel format requested.\n");
			return(NULL);
			break;			
	}

//	pixFormat = k8IndexedGrayPixelFormat;
//	bytesPerPixel = 1l;

	message( "checking that quicktime is installed..." );

	// check that quicktime is installed.
	if( ( err_s = Gestalt( gestaltQuickTimeVersion, &qtVersion ) ) != noErr )
	{
		gen_fatal( "QuickTime is not installed!! (%d).", err_s );
	}
	else
	{
		message( "Quicktime is installed" );
		message( "checking quicktime version..." );

		// check the quicktime version.
		if( ( qtVersion >> 16 ) < 0x640 )
		{
			gen_fatal( "QuickTime version 6.4 or newer is required, found %ld", qtVersion );
		}
		else
		{
			message( "QuickTime version okay, found version: %ld", qtVersion );
		}
	}

	// Initialize the Movie Toolbox and creates a private storage area
	if( ( err_s = EnterMovies() ) != noErr )
	{
		gen_fatal( "Unable to initialise Carbon/QuickTime (%d).", err_s );
	}
	else
	{
		message( "Initialized the Movie Toolbox" );
	}

	// increment the video device count in use.
	gVidCount++;

	// Allocate some memory for the video parameters.
	arMalloc( vid, AR2VideoParamT, 1 );

	// initialize the memory with zeros
	memset( vid, 0, sizeof( AR2VideoParamT ) );

	// set the status
	vid->status = 0;
	
	// copy the FPS that was defined previously
	vid->showFPS = showFPS;

	// set the frame count.
	vid->frameCount = 0;

	// the last time we grabbed a frame??
	vid->lastTime = 0;

	// not sure??
	vid->timeScale = 0;

	//
	vid->grabber = grabber;

	// attempt to allocate memory for a grabber and initialize it.
	if( !( vid->pVdg = vdgAllocAndInit( grabber ) ) )
	{
		gen_fatal( "Unable to Initialize the grabber: err=%ld", err );
	}
	else
	{
		message( "allocated memory for the grabber and initialized it" );
	}

	// show the dialog if it has been requested.
	err = vdgRequestSettings( vid->pVdg, showDialog, gVidCount );

	if( err != 0 )
	{
		gen_fatal( "vdgRequestSettings err=%ld", err);
	}
	else
	{
		message( "Settings received okay" );
	}

	// attemp to preflight the grabbing?
	vdgPreflightGrabbing( vid->pVdg );

	// Set the timer frequency from the Vdig's Data Rate
	// ASC soft vdig fails this call
	vdgGetDataRate( vid->pVdg, &vid->milliSecPerFrame, &vid->frameRate, &vid->bytesPerSecond);

	message( "milliseconds per frame is now = %ld", vid->milliSecPerFrame );
	message( "framerate is now = %f", FixedToFloat( vid->frameRate ) );

	// Some vdigs do return the frameRate but not the milliSecPerFrame
	if( ( vid->milliSecPerFrame == 0 ) && ( vid->frameRate != 0 ) )
	{
		message( "adjusting the the milliSecPerFrame" );
		vid->milliSecPerFrame = (1000L << 16) / vid->frameRate;
	} 

	message( "milliseconds per frame is now = %ld", vid->milliSecPerFrame );
	message( "framerate is now = %f", FixedToFloat( vid->frameRate ) );

	// Poll the vdig at twice the frame rate or between sensible limits
	vid->milliSecPerTimer = vid->milliSecPerFrame / 2;

	message( "vid->milliSecPerTimer  = %ld", vid->milliSecPerTimer );

	if (vid->milliSecPerTimer <= 0) {
		fprintf(stderr, "vid->milliSecPerFrame: %ld ", vid->milliSecPerFrame);
		vid->milliSecPerTimer = AR_VIDEO_IDLE_INTERVAL_MILLISECONDS_MIN;
		fprintf(stderr, "forcing timer period to %ldms\n", vid->milliSecPerTimer);
	}
	if (vid->milliSecPerTimer >= AR_VIDEO_IDLE_INTERVAL_MILLISECONDS_MAX) {
		fprintf(stderr, "vid->milliSecPerFrame: %ld ", vid->milliSecPerFrame);
		vid->milliSecPerFrame = AR_VIDEO_IDLE_INTERVAL_MILLISECONDS_MAX;
		fprintf(stderr, "forcing timer period to %ldms\n", vid->milliSecPerTimer);
	}

	vid->vdImageDesc = (ImageDescriptionHandle)NewHandle(0);
	if (err = vdgGetImageDescription(vid->pVdg, vid->vdImageDesc)) {
		fprintf(stderr, "ar2VideoOpen(): vdgGetImageDescription err=%ld\n", err);
		goto out3;
	}

	// Report video size and compression type.
	message( "Video cType is %c%c%c%c, size is %dx%d",
			(char)(((*(vid->vdImageDesc))->cType >> 24) & 0xFF),
			(char)(((*(vid->vdImageDesc))->cType >> 16) & 0xFF),
			(char)(((*(vid->vdImageDesc))->cType >>  8) & 0xFF),
			(char)(((*(vid->vdImageDesc))->cType >>  0) & 0xFF),
			((*vid->vdImageDesc)->width), ((*vid->vdImageDesc)->height));

	// If a particular size was requested, set the size of the GWorld to
	// the request, otherwise set it to the size of the incoming video.
	vid->width = (width ? width : (int)((*vid->vdImageDesc)->width));
	vid->height = (height ? height : (int)((*vid->vdImageDesc)->height));
	SetRect(&(vid->theRect), 0, 0, (short)vid->width, (short)vid->height);

	// Make a scaling matrix for the sequence if size of incoming video differs from GWorld dimensions.
	if (vid->width != (int)((*vid->vdImageDesc)->width) || vid->height != (int)((*vid->vdImageDesc)->height)) {
		sourceRect.right = (*vid->vdImageDesc)->width;
		sourceRect.bottom = (*vid->vdImageDesc)->height;
		arMalloc(vid->scaleMatrixPtr, MatrixRecord, 1);
		RectMatrix(vid->scaleMatrixPtr, &sourceRect, &(vid->theRect));
		fprintf(stdout, "Video will be scaled to size %dx%d.\n", vid->width, vid->height);
	} else {
		vid->scaleMatrixPtr = NULL;
	}

	// Allocate buffer for the grabber to write pixel data into, and use
	// QTNewGWorldFromPtr() to wrap an offscreen GWorld structure around
	// it. We do it in these two steps rather than using QTNewGWorld()
	// to guarantee that we don't get padding bytes at the end of rows.
	vid->rowBytes = vid->width * bytesPerPixel;
	vid->bufSize = vid->height * vid->rowBytes;
	//	if (!(vid->bufPixels = (ARUint8 *)valloc(vid->bufSize * sizeof(ARUint8)))) exit (1);
	if (!(vid->bufPixels = ( unsigned char* )valloc(vid->bufSize * sizeof( unsigned char )))) exit (1);
#ifdef AR_VIDEO_DEBUG_BUFFERCOPY
	// And another two buffers for OpenGL to read out of.
	if (!(vid->bufPixelsCopy1 = ( unsigned char* )valloc(vid->bufSize * sizeof( unsigned char )))) exit (1);
	//	if (!(vid->bufPixelsCopy1 = (ARUint8 *)valloc(vid->bufSize * sizeof(ARUint8)))) exit (1);
	//	if (!(vid->bufPixelsCopy2 = (ARUint8 *)valloc(vid->bufSize * sizeof(ARUint8)))) exit (1);
	if (!(vid->bufPixelsCopy2 = ( unsigned char* )valloc(vid->bufSize * sizeof( unsigned char )))) exit (1);
#endif // AR_VIDEO_DEBUG_BUFFERCOPY
	// Wrap a GWorld around the pixel buffer.
	err_s = QTNewGWorldFromPtr(&(vid->pGWorld),			// returned GWorld
			pixFormat,				// format of pixels
			&(vid->theRect),			// bounds
			0,						// color table
			NULL,					// GDHandle
			0,						// flags
			(void *)(vid->bufPixels), // pixel base addr
			vid->rowBytes);			// bytes per row
	if (err_s != noErr) {
		fprintf(stderr,"ar2VideoOpen(): Unable to create offscreen buffer for sequence grabbing (%d).\n", err_s);
		goto out5;
	}

	// Lock the pixmap and make sure it's locked because
	// we can't decompress into an unlocked PixMap, 
	// and open the default sequence grabber.
	// TODO: Allow user to configure sequence grabber.
	err_i = (int)LockPixels(GetGWorldPixMap(vid->pGWorld));
	if (!err_i) {
		fprintf(stderr,"ar2VideoOpen(): Unable to lock buffer for sequence grabbing.\n");
		goto out6;
	}

	// Erase to black.
	GetGWorld(&theSavedPort, &theSavedDevice);    
	SetGWorld(vid->pGWorld, NULL);
	BackColor(blackColor);
	ForeColor(whiteColor);
	EraseRect(&(vid->theRect));
	SetGWorld(theSavedPort, theSavedDevice);

	// Set the decompression destination to the offscreen GWorld.
	if (err_s = vdgSetDestination(vid->pVdg, vid->pGWorld)) {
		fprintf(stderr, "ar2VideoOpen(): vdgSetDestination err=%d\n", err_s);
		goto out6;	
	}

	// Initialise per-vid pthread variables.
	// Create a mutex to protect access to data structures.
	if( ( err_i = pthread_mutex_init( &( vid->bufMutex ), NULL ) ) != 0 )
	{
		// check the error value
		if( err_i == EINVAL )
		{
			gen_fatal( "NULL is not a valid value for pthread_mutex_init" );
		}
		else if( err_i == ENOMEM )
		{
			gen_fatal( "The process cannot allocate enough memory to create another mutex" );
		}
		else if( err_i == EAGAIN )
		{
			gen_fatal( "This system temporarily lacks the resources to create another mutex" );
		}
		else
		{
			gen_fatal( "Unspecified error - cannot initialize mutex" );
		}
	}
	else
	{
		message( "Successfully created a new mutex with default attributes" );
	}
	// Create condition variable.
	if (err_i == 0) {
		if ((err_i = pthread_cond_init(&(vid->condition), NULL)) != 0) {
			fprintf(stderr, "ar2VideoOpen(): Error %d creating condition variable.\n", err_i);
			pthread_mutex_destroy(&(vid->bufMutex));
		}
	}

	if (err_i != 0) { // Clean up component on failure to init per-vid pthread variables.
		goto out6;
	}

	goto out;

out6:
	DisposeGWorld(vid->pGWorld);
out5:
#ifdef AR_VIDEO_DEBUG_BUFFERCOPY
	free(vid->bufPixelsCopy2);
	free(vid->bufPixelsCopy1);
#endif // AR_VIDEO_DEBUG_BUFFERCOPY
	free(vid->bufPixels);
	if (vid->scaleMatrixPtr) free(vid->scaleMatrixPtr);
out3:
	DisposeHandle((Handle)vid->vdImageDesc);
out2:	
	vdgReleaseAndDealloc(vid->pVdg);
out:
	return (vid);
}

int ar2VideoClose(AR2VideoParamT *vid)
{
	int err_i;

	// Destroy the condition variable.
	if ((err_i = pthread_cond_destroy(&(vid->condition))) != 0) {
		fprintf(stderr, "ar2VideoClose(): Error %d destroying condition variable.\n", err_i);
	}

	// Destroy the mutex.
	if ((err_i = pthread_mutex_destroy(&(vid->bufMutex))) != 0) {
		fprintf(stderr, "ar2VideoClose(): Error %d destroying mutex.\n", err_i);
	}

	if (vid->pGWorld != NULL) {
		DisposeGWorld(vid->pGWorld);
		vid->pGWorld = NULL;
	}

#ifdef AR_VIDEO_DEBUG_BUFFERCOPY
	if (vid->bufPixelsCopy2) {
		free(vid->bufPixelsCopy2);
		vid->bufPixelsCopy2 = NULL;
	}
	if (vid->bufPixelsCopy1) {
		free(vid->bufPixelsCopy1);
		vid->bufPixelsCopy1 = NULL;
	}
#endif // AR_VIDEO_DEBUG_BUFFERCOPY
	if (vid->bufPixels) {
		free(vid->bufPixels);
		vid->bufPixels = NULL;
	}

	if (vid->scaleMatrixPtr) {
		free(vid->scaleMatrixPtr);
		vid->scaleMatrixPtr = NULL;
	}

	if (vid->vdImageDesc) {
		DisposeHandle((Handle)vid->vdImageDesc);
		vid->vdImageDesc = NULL;
	}

	vdgReleaseAndDealloc(vid->pVdg);

	// Count one less grabber running.
	free (vid);
	gVidCount--;

	// If we're the last to close, clean up after everybody.
	if (!gVidCount) {
		// Probably a good idea to close down QuickTime.
		ExitMovies();
	}

	return (0);
}

/**
 * MakeSequenceGrabber - creates an instance of the component on the node
 * specified by grabber
 **/
//SeqGrabComponent MakeSequenceGrabber(WindowRef pWindow, const int grabber)
SeqGrabComponent MakeSequenceGrabber( const int grabber )
{
	// A ComponentInstance is a pointer to a ComponentInstanceRecord, which is turn 
	// basically has one field "data" that is a pointer to a long
	ComponentInstance seqGrab = NULL;
	ComponentResult	err = noErr;

	ComponentDescription cDesc;

	// Open the sequence grabber.
	// A four-character code that identifies the type of component, in this
	// case SeqGrabComponentType is = 'barg'. All components of a particular 
	// type support a common set of interface routines. This field is used 
	// to search for components of a given type "barg"
	cDesc.componentType = SeqGrabComponentType;

	// Could use subtype vdSubtypeIIDC for IIDC-only cameras (i.e. exclude 
	// DV and other cameras.), but we are not interested in restricting to
	// subtypes at this point.
	cDesc.componentSubType = 0L; 

	// can use this field to select devices based on manufacturer. TODO - 
	// use thid field to check if it is a ptgrey camera.
	cDesc.componentManufacturer = 0L;

	// used to indicate a components features
	cDesc.componentFlags = 0L;

	// ignore all flags.
	cDesc.componentFlagsMask = 0L;

	// find out how many components fit the criteria that we set.
	long cCount = CountComponents( &cDesc );
	message( "Opening sequence grabber %d of %ld", grabber, cCount );

	// initialize a component object
	Component c = 0;

	// attempt to find the grabber that we are interested in.
	for( int i = 1; (c = FindNextComponent(c, &cDesc)) != 0; i++ )
	{
		// Could call GetComponentInfo() here to get more info on this 
		// SeqGrabComponentType component. Is this the grabber 
		// requested?
		if (i == grabber)
		{
			// this doesn't appear to do anything useful, it does 
			// get me the manufacturer, but both cameras I have
			// tried return the same manufacturer type ie = 'appl'
			GetComponentInfo( c, &cDesc, NULL, NULL, NULL );

			message( "component type: %x", cDesc.componentType );
			message( "component sub type: %x", cDesc.componentSubType );
			message( "component manufacturer: %x", cDesc.componentManufacturer );
			message( "component flags: %x", cDesc.componentFlags );
			message( "component flags mask: %x", cDesc.componentFlagsMask );

			// create an instance of the component.
			seqGrab = OpenComponent(c);
		}
	}

	// check if a sequence grabber was obtained.
	if( !seqGrab )
	{
		gen_fatal( "Failed to open a sequence grabber component." );
	}

	// initialize the default sequence grabber component
	if( ( err = SGInitialize( seqGrab ) ) )
	{
		gen_fatal( "SGInitialize err=%ld", err);
	}

	// specify the destination data reference for a record operation
	// tell it we're not making a movie if the flag seqGrabDontMakeMovie 
	// is used, the sequence grabber still calls your data function, but 
	// does not write any data to the movie file writeType will always be 
	// set to seqGrabWriteAppend
	if( ( err = SGSetDataRef( seqGrab, 0, 0, seqGrabDontMakeMovie ) ) )
	{
		gen_fatal( "SGSetDataRef err=%ld", err );
	}

	// return the sequence grabber.
	return seqGrab;
}

/**
 * create a channel and define the usage such that the camera only every returns
 * the freshest image available, this way we aren't dealing with out of date 
 * images and we don't get any lag.
 **/
void MakeSequenceGrabChannel(SeqGrabComponent seqGrab, SGChannel* psgchanVideo)
{
	// return code
	ComponentResult err = noErr;

	// set up a video type channel (as opposed to a sound channel) puts the
	// value into psgchanVideo
	if( ( err = SGNewChannel( seqGrab, VideoMediaType, psgchanVideo ) ) )
	{
		gen_fatal( "SGNewChannel err=%ld", err );
	}

	// set up the channel so that it always returns the freshest frame.
	if( ( err = SGSetChannelUsage( *psgchanVideo, seqGrabLowLatencyCapture ) ) )
	{
		gen_fatal( "SGSetChannelUsage err=%ld", err );
	}
}

/**
 * create a sequence grabbing device, create the channel and define it's 
 * properties
 **/
VdigGrabRef vdgAllocAndInit(const int grabber)
{
	VdigGrabRef pVdg = NULL;
	OSErr err;

	// Allocate the grabber structure
	arMalloc( pVdg, VdigGrab, 1 );

	// initialize the memory to zeros.
	memset( pVdg, 0, sizeof( VdigGrab ) );	

	// attempt to make a sequence grabber.
	if( !( pVdg->seqGrab = MakeSequenceGrabber( grabber ) ) )
	{
		gen_fatal( "Couldn't make a sequence grabber" ); 
	}

	// set up the sequence channel and define its properties.
	MakeSequenceGrabChannel( pVdg->seqGrab, &pVdg->sgchanVideo );

	return pVdg;
}

ComponentResult vdgRequestSettings(VdigGrab* pVdg, const int showDialog, const int inputIndex)
{
	ComponentResult err;

	// Use the SG Dialog to allow the user to select device and compression 
	// settings, this calls the objective c code.
	err = RequestSGSettings( inputIndex, pVdg->seqGrab, pVdg->sgchanVideo, showDialog );

	// check the return value
	if( err != 0 )
	{
		gen_fatal( "RequestSGSettings err=%ld", err );
	}
	else
	{
		message( "Settings requested okay" );
	}

	// get the component instance that identifies the connection between 
	// the video channel component and its video digitizer component.
	// SGGetVideoDigitizerComponent allows the sequence grabber component 
	// to determine the video digitizer component that is providing source 
	// video to the video channel component
	pVdg->vdCompInst = SGGetVideoDigitizerComponent( pVdg->sgchanVideo );

	// make sure something was returned.
	if( pVdg->vdCompInst == NULL )
	{
		gen_fatal( "SGGetVideoDigitizerComponent error" );
	}

	return err;
}

VideoDigitizerError vdgGetDeviceNameAndFlags(VdigGrab* pVdg, char* szName, long* pBuffSize, UInt32* pVdFlags)
{
	VideoDigitizerError err;
	Str255	vdName; // Pascal string (first byte is string length).
	UInt32	vdFlags;

	if (!pBuffSize) {
		fprintf(stderr, "vdgGetDeviceName: NULL pointer error\n");
		err = (VideoDigitizerError)qtParamErr; 
		goto endFunc;
	}

	if (err = VDGetDeviceNameAndFlags(  pVdg->vdCompInst,
				vdName,
				&vdFlags)) {
		fprintf(stderr, "VDGetDeviceNameAndFlags err=%ld\n", err);
		*pBuffSize = 0; 
		goto endFunc;
	}

	if (szName) {
		int copyLen = (*pBuffSize-1 < vdName[0] ? *pBuffSize-1 : vdName[0]);

		strncpy(szName, (char *)vdName+1, copyLen);
		szName[copyLen] = '\0';

		*pBuffSize = copyLen + 1;
	} else {
		*pBuffSize = vdName[0] + 1;
	} 

	if (pVdFlags)
		*pVdFlags = vdFlags;

endFunc:
	return err;
}

OSErr vdgSetDestination(  VdigGrab* pVdg,
		CGrafPtr  dstPort )
{
	pVdg->dstPort = dstPort;
	return noErr;
}

/**
 * set a bunch of properties to do with compression and image properties.
 **/
void vdgPreflightGrabbing(VdigGrab* pVdg)
{
	VideoDigitizerError err;
	Rect maxRect;

	DigitizerInfo info;

	// Returns capability and status information about a specified video 
	// digitizer component.
	err = VDGetDigitizerInfo( pVdg->vdCompInst, &info );

	// check the return value to see if there was an error.
	if( err !=  noErr )
	{
		gen_fatal( "VDGetDigitizerInfo err=%ld", err);
	}
	else 
	{
		message( "got digitizer information" );

		//check what type of video digitizer component we got.
		if( info.vdigType == vdTypeBasic )
		{
			message( "Basic Video digitizer; does not support any clipping" );
		}
		else if( info.vdigType == vdTypeAlpha )
		{
			message( "Supports clipping by means of an alpha channel" );
		}
		else if( info.vdigType == vdTypeMask )
		{
			message( "Supports clipping by means of a mask plane" );
		}
		else if( info.vdigType == vdTypeKey )
		{
			message( "Supports clipping by means of key colours" );
		}

		if( info.outputCapabilityFlags & digiOutDoes1 )
		{
			message( "the video digitizer component can work with pixel maps that contain 1-bit pixels" );
		}
		if( info.outputCapabilityFlags & digiOutDoes2 )
		{
			message( "the video digitizer component can work with pixel maps that contain 2-bit pixels" );
		}
		if( info.outputCapabilityFlags & digiOutDoes4 )
		{
			message( "the video digitizer component can work with pixel maps that contain 4-bit pixels" );
		}
		if( info.outputCapabilityFlags & digiOutDoes8 )
		{
			message( "the video digitizer component can work with pixel maps that contain 8-bit pixels" );
		}
		if( info.outputCapabilityFlags & digiOutDoes16 )
		{
			message( "the video digitizer component can work with pixel maps that contain 16-bit pixels" );
		}
		if( info.outputCapabilityFlags & digiOutDoes32 )
		{
			message( "the video digitizer component can work with pixel maps that contain 32-bit pixels" );
		}
		if( info.outputCapabilityFlags & digiOutDoesDither )
		{
			message( "the video digitizer component supports dithering" );
		}
		if( info.outputCapabilityFlags & digiOutDoesStretch )
		{
			message( "the video digitizer component can stretch images to arbitrary sizes" );
		}
		if( info.outputCapabilityFlags & digiOutDoesShrink )
		{
			message( "the video digitizer component can shrink images to arbitrary sizes" );
		}
		if( info.outputCapabilityFlags & digiOutDoesMask )
		{
			message( "the video digitizer component can handle clipping regions" );
		}
		if( info.outputCapabilityFlags & digiOutDoesDouble )
		{
			message( "the video digitizer component supports stretching to quadruple size when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesQuad )
		{
			message( "the video digitizer component supports stretching an image to 16 times its original size when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesQuarter )
		{
			message( "the video digitizer component can shrink an image to one-quarter of its original size when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesSixteenth )
		{
			message( "the video digitizer component can shrink an image to 1/16 of its original size when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesRotate )
		{
			message( "the video digitizer component can rotate an image when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesHorizFlip )
		{
			message( "the video digitizer component can flip an image horizontally when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesVertFlip )
		{
			message( "the video digitizer component can flip an image vertically when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesSkew )
		{
			message( "the video digitizer component can skew an image when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesBlend )
		{
			message( "the video digitizer component can blend the resulting image with a matte when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesWarp )
		{
			message( "the video digitizer component can warp an image when displaying the output video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesHWPlayThru )
		{
			message( "the video digitizer component does not need idle time in order to display its video" );
		}
		if( info.outputCapabilityFlags & digiOutDoesILUT )
		{
			message( "the video digitizer component supports inverse lookup tables for indexed color modes" );
		}
		if( info.outputCapabilityFlags & digiOutDoesKeyColor )
		{
			message( "the video digitizer component supports clipping by means of key colors" );
		}
		if( info.outputCapabilityFlags & digiOutDoesAsyncGrabs )
		{
			message( "the video digitizer component can operate asynchronously" );
		}
		if( info.outputCapabilityFlags & digiOutDoesUnreadableScreenBits )
		{
			message( "the video digitizer may place pixels on the screen that cannot be used when compressing images" );
		}
		if( info.outputCapabilityFlags & digiOutDoesCompress )
		{
			message( "the video digitizer component supports compressed source devices" );
		}
		if( info.outputCapabilityFlags & digiOutDoesCompressOnly )
		{
			message( "the video digitizer component only provides compressed image data" );
		}
		if( info.outputCapabilityFlags & digiOutDoesPlayThruDuringCompress )
		{
			message( "the video digitizer component can draw images on the screen at the same time that it is delivering compressed image data" );
		}

		// we only want to work with compressed types so check if the
		// camera supports compressed types.
		if( !( info.outputCapabilityFlags & digiOutDoesCompress ) )
		{
			gen_fatal( "camera does not support compressed types. This code only works with compressed types at the moment" );
		}
	}

	// create a new list handle
	VDCompressionListHandle h = (VDCompressionListHandle)NewHandle(0);

	// get the compression types that this camera supports
	long result = VDGetCompressionTypes( pVdg->vdCompInst, h );

	// check that we got the list okay.
	if( result != noErr )
	{
		gen_fatal( "couldn't get the compression types. Error no: %dl", result );
	}

	// find out how many codec we have.
	int numOfTypes = sizeof( **h ) / sizeof( VDCompressionList );

	// display the number of codecs that we found.
	message( "Found %d codec(s)", numOfTypes );

	// go through the list of codecs supported and display infor about them
	for( int i = 0; i < numOfTypes; i++ )
	{
		VDCompressionList cl = ( VDCompressionList )*h[ i ];

		message( "cType: %x", cl.cType );
		message( "Compressor name: %s", cl.name );
		message( "Compression algorithm: %s", cl.typeName );
	}

	// Tell the vDig we're starting to change several settings.
	// Apple's SoftVDig doesn't seem to like these calls.
	err = VDCaptureStateChanging( pVdg->vdCompInst, vdFlagCaptureSetSettingsBegin );

	// check that the VDIG knows what we are on about.
	if( err != noErr ) 
	{
		gen_fatal( "Cannot change the state." );
	}

	// find out the maximum source rectangle
	err = VDGetMaxSrcRect( pVdg->vdCompInst, currentIn, &maxRect );

	if( err != noErr )
	{
		gen_fatal( "There was a problem grabbing the maximum source rectangle" );
	}
	else
	{
		message( "Maximum source rectangle is { ( %d, %d ), ( %d, %d ) }", maxRect.left, maxRect.top, maxRect.right, maxRect.bottom );
	}

	// Try to set maximum capture size ... is this necessary as we're setting the 
	// rectangle in the VDSetCompression call later?  I suppose that it is, as
	// we're setting digitization size rather than compression size here...
	// Apple vdigs don't like this call	
	err = VDSetDigitizerRect( pVdg->vdCompInst, &maxRect);

	// check for errors.
	if( err != noErr )
	{
		gen_fatal( "couldn't set max rectangle" );
	}

	// make sure we are using compression, this should be on by default
	err = VDSetCompressionOnOff( pVdg->vdCompInst, true );

	// check for errors.
	if( err != noErr )
	{
		gen_fatal( "Couldn't tell the VDIG to use compression" );
	}

	// attempt to set the framerate, setting it to zero is supposed to 
	// use the digitizers default framerate
	err = VDSetFrameRate( pVdg->vdCompInst, 0 );

	if( err != noErr )
	{
		gen_fatal( "Couldn't set framerate" );
	}

	// try to set a format that matches our target
	// necessary for ASC softvdig (even if it doesn't support
	// the requested codec)
	// note that for the Apple IIDC vdig in 10.3 if we request yuv2 explicitly
	// we'll get 320x240 frames returned but if we leave codecType as 0
	// we'll get 640x480 frames returned instead (which use 4:1:1 encoding on
	// the wire rather than 4:2:2)
	err = VDSetCompression(pVdg->vdCompInst,
			0, //kComponentVideoCodecType, //0, //'yuv2'
			0,	
			&maxRect, 
			0, //codecNormalQuality,
			0, //codecNormalQuality,
			0);
	// check for an error
	if( err != noErr )
	{
		gen_fatal( "couldn't set compression type" );
	}

	// tell the camera not to use old frames.
	err = VDCaptureStateChanging( pVdg->vdCompInst, vdFlagCaptureLowLatency );

	// check for an error
	if( err != noErr )
	{
		gen_fatal( "couldn't tell the digitizer to only use the freshest frames" );
	}

	// Tell the vDig we've finished changing settings.
	err = VDCaptureStateChanging( pVdg->vdCompInst, vdFlagCaptureSetSettingsEnd );

	// check for an error
	if( err != noErr )
	{
		gen_fatal( "couldn't tell the digitizer that we have finished applying our settings" );
	}

	pVdg->vdImageDesc = (ImageDescriptionHandle)NewHandle(0);

	// grab a description of the image
	err = VDGetImageDescription( pVdg->vdCompInst, pVdg->vdImageDesc );

	// check for an error
	if( err != noErr )
	{
		gen_fatal( "couldn't grab image description" );
	}

	message( "obtained image description" );
	message( "image idSize = %dl", ( **( pVdg->vdImageDesc ) ).idSize );
	message( "version = %d", ( ** ( pVdg->vdImageDesc ) ).version );
	message( "width = %d", ( ** ( pVdg->vdImageDesc ) ).width );
	message( "height = %d", ( ** ( pVdg->vdImageDesc ) ).height );

	// From Steve Sisak: find out if Digitizer is cropping for you.
	err = VDGetDigitizerRect( pVdg->vdCompInst, &pVdg->vdDigitizerRect );

	if( err != noErr )
	{
		gen_fatal( "couldn't determine if the digitizer was cropping for us" );
	}

	pVdg->isPreflighted = 1;
}

void vdgGetDataRate(	VdigGrab* pVdg, 
			long* pMilliSecPerFrame,
			Fixed* pFramesPerSecond,
			long* pBytesPerSecond )
{
	// Retrieves information that describes the performance capabilities 
	// of a video digitizer.
	VideoDigitizerError err = VDGetDataRate( pVdg->vdCompInst, 
			pMilliSecPerFrame,
			pFramesPerSecond,
			pBytesPerSecond );

	// check for an error
	if( err != noErr )
	{
		gen_fatal( "couldn't get the data rate" );
	}

	message( "milliseconds per frame = %ld", *pMilliSecPerFrame );
	message( "framerate is = %f", FixedToFloat( *pFramesPerSecond ) );
	message( "expected bytes per second = %ld", *pBytesPerSecond );
}

VideoDigitizerError vdgGetImageDescription( VdigGrab* pVdg,
		ImageDescriptionHandle vdImageDesc )
{
	VideoDigitizerError err;

	if (err = VDGetImageDescription( pVdg->vdCompInst, vdImageDesc))
	{
		fprintf(stderr, "VDGetImageDescription err=%ld\n", err);
		goto endFunc;		
	}

endFunc:	
	return err;
}

OSErr vdgDecompressionSequenceBegin(  VdigGrab* pVdg,
		CGrafPtr dstPort, 
		Rect* pDstRect,
		MatrixRecord* pDstScaleMatrix )
{
	OSErr err;

	// 	Rect				   sourceRect = pMungData->bounds;
	//	MatrixRecord		   scaleMatrix;	

	// !HACK! Different conversions are used for these two equivalent types
	// so we force the cType so that the more efficient path is used
	if ((*pVdg->vdImageDesc)->cType == FOUR_CHAR_CODE('yuv2'))
		(*pVdg->vdImageDesc)->cType = FOUR_CHAR_CODE('yuvu'); // kYUVUPixelFormat

	// make a scaling matrix for the sequence
	//	sourceRect.right = (*pVdg->vdImageDesc)->width;
	//	sourceRect.bottom = (*pVdg->vdImageDesc)->height;
	//	RectMatrix(&scaleMatrix, &sourceRect, &pMungData->bounds);

	// begin the process of decompressing a sequence of frames
	// this is a set-up call and is only called once for the sequence - the ICM will interrogate different codecs
	// and construct a suitable decompression chain, as this is a time consuming process we don't want to do this
	// once per frame (eg. by using DecompressImage)
	// for more information see Ice Floe #8 http://developer.apple.com/quicktime/icefloe/dispatch008.html
	// the destination is specified as the GWorld
	if (err = DecompressSequenceBeginS(    &pVdg->dstImageSeq,	// pointer to field to receive unique ID for sequence.
				pVdg->vdImageDesc,	// handle to image description structure.
				0,					// pointer to compressed image data.
				0,					// size of the buffer.
				dstPort,				// port for the DESTINATION image
				NULL,				// graphics device handle, if port is set, set to NULL
				NULL, //&sourceRect  // source rectangle defining the portion of the image to decompress 
				pDstScaleMatrix,		// transformation matrix
				srcCopy,				// transfer mode specifier
				(RgnHandle)NULL,		// clipping region in dest. coordinate system to use as a mask
				0L,					// flags
				codecHighQuality, //codecNormalQuality   // accuracy in decompression
				bestSpeedCodec)) //anyCodec  bestSpeedCodec  // compressor identifier or special identifiers ie. bestSpeedCodec
	{
		fprintf(stderr, "DecompressSequenceBeginS err=%d\n", err);
		goto endFunc;
	}

endFunc:	
	return err;
}

OSErr vdgDecompressionSequenceWhen(   VdigGrab* pVdg,
		Ptr theData,
		long dataSize)
{
	OSErr err;
	CodecFlags	ignore = 0;

	if(err = DecompressSequenceFrameWhen(   pVdg->dstImageSeq,	// sequence ID returned by DecompressSequenceBegin.
				theData,			// pointer to compressed image data.
				dataSize,			// size of the buffer.
				0,					// in flags.
				&ignore,			// out flags.
				NULL,				// async completion proc.
				NULL ))				// frame timing information.
	{
		fprintf(stderr, "DecompressSequenceFrameWhen err=%d\n", err);
		goto endFunc;
	}

endFunc:
	return err;
}

OSErr vdgDecompressionSequenceEnd( VdigGrab* pVdg )
{
	OSErr err;

	if (!pVdg->dstImageSeq)
	{
		fprintf(stderr, "vdgDestroyDecompressionSequence NULL sequence\n");
		err = qtParamErr; 
		goto endFunc;
	}

	if (err = CDSequenceEnd(pVdg->dstImageSeq))
	{
		fprintf(stderr, "CDSequenceEnd err=%d\n", err);
		goto endFunc;
	}

	pVdg->dstImageSeq = 0;

endFunc:
	return err;
}

VideoDigitizerError vdgStartGrabbing(VdigGrab*   pVdg, MatrixRecord* pDstScaleMatrix)
{
	VideoDigitizerError err;

	if (!pVdg->isPreflighted)
	{
		fprintf(stderr, "vdgStartGrabbing called without previous successful vdgPreflightGrabbing()\n");
		err = (VideoDigitizerError)badCallOrderErr; 
		goto endFunc;	
	}

	if (err = VDCompressOneFrameAsync( pVdg->vdCompInst ))
	{
		fprintf(stderr, "VDCompressOneFrameAsync err=%ld\n", err);
		goto endFunc;	
	}

	if (err = vdgDecompressionSequenceBegin( pVdg, pVdg->dstPort, NULL, pDstScaleMatrix ))
	{
		fprintf(stderr, "vdgDecompressionSequenceBegin err=%ld\n", err);
		goto endFunc;	
	}

	pVdg->isGrabbing = 1;

endFunc:
	return err;
}

VideoDigitizerError vdgStopGrabbing(VdigGrab* pVdg)
{
	VideoDigitizerError err;

	if (err = VDSetCompressionOnOff( pVdg->vdCompInst, 0))
	{
		fprintf(stderr, "VDSetCompressionOnOff err=%ld\n", err);
		//		goto endFunc;		
	}

	if (err = (VideoDigitizerError)vdgDecompressionSequenceEnd(pVdg))
	{
		fprintf(stderr, "vdgDecompressionSequenceEnd err=%ld\n", err);
		//		goto endFunc;
	}

	pVdg->isGrabbing = 0;

	//endFunc:
	return err;
}

bool vdgIsGrabbing(VdigGrab* pVdg)
{
	return pVdg->isGrabbing;
}

VideoDigitizerError vdgPoll(	VdigGrab*   pVdg,
		UInt8*		pQueuedFrameCount,
		Ptr*		pTheData,
		long*		pDataSize,
		UInt8*		pSimilarity,
		TimeRecord*	pTime )
{
	VideoDigitizerError err;

	if (!pVdg->isGrabbing)
	{ 
		fprintf(stderr, "vdgGetFrame error: not grabbing\n");
		err = (VideoDigitizerError)qtParamErr; 
		goto endFunc;
	}

	if (err = VDCompressDone(	pVdg->vdCompInst,
				pQueuedFrameCount,
				pTheData,
				pDataSize,
				pSimilarity,
				pTime ))
	{
		fprintf(stderr, "VDCompressDone err=%ld\n", err);
		goto endFunc;
	}

	// Overlapped grabbing
	if (*pQueuedFrameCount)
	{
		if (err = VDCompressOneFrameAsync(pVdg->vdCompInst))
		{
			fprintf(stderr, "VDCompressOneFrameAsync err=%ld\n", err);
			goto endFunc;		
		}
	}

endFunc:
	return err;
}

VideoDigitizerError vdgReleaseBuffer(VdigGrab*   pVdg, Ptr theData)
{
	VideoDigitizerError err;

	if (err = VDReleaseCompressBuffer(pVdg->vdCompInst, theData))
	{
		fprintf(stderr, "VDReleaseCompressBuffer err=%ld\n", err);
		goto endFunc;		
	}

endFunc:
	return err;
}

VideoDigitizerError vdgIdle(VdigGrab* pVdg, int*  pIsUpdated)
{
	VideoDigitizerError err;

	UInt8 		queuedFrameCount;
	Ptr			theData;
	long		dataSize;
	UInt8		similarity;
	TimeRecord	time;

	*pIsUpdated = 0;

	// should be while?
	if ( !(err = vdgPoll( pVdg,
					&queuedFrameCount,
					&theData,
					&dataSize,
					&similarity,
					&time))
			&& queuedFrameCount)
	{
		*pIsUpdated = 1;

		// Decompress the sequence
		if (err = (VideoDigitizerError)vdgDecompressionSequenceWhen( pVdg,
					theData,
					dataSize))
		{
			fprintf(stderr, "vdgDecompressionSequenceWhen err=%ld\n", err);
			//			goto endFunc;	
		}

		// return the buffer
		if(err = vdgReleaseBuffer(pVdg, theData))
		{
			fprintf(stderr, "vdgReleaseBuffer err=%ld\n", err);
			//			goto endFunc;
		}
	}

	if (err)
	{
		fprintf(stderr, "vdgPoll err=%ld\n", err);
		goto endFunc;
	}

endFunc:
	return err;
}

ComponentResult vdgReleaseAndDealloc(VdigGrab* pVdg)
{
	ComponentResult err = noErr;		

	if (pVdg->vdImageDesc) {
		DisposeHandle((Handle)pVdg->vdImageDesc);
		pVdg->vdImageDesc = NULL;
	}

	if (pVdg->vdCompInst) {
		if (err = CloseComponent(pVdg->vdCompInst))
			fprintf(stderr, "CloseComponent err=%ld\n", err);		
		pVdg->vdCompInst = NULL;
	}

	if (pVdg->sgchanVideo) {
		if (err = SGDisposeChannel(pVdg->seqGrab, pVdg->sgchanVideo))
			fprintf(stderr, "SGDisposeChannel err=%ld\n", err);	
		pVdg->sgchanVideo = NULL;
	}

	if (pVdg->seqGrab) {
		if (err = CloseComponent(pVdg->seqGrab))
			fprintf(stderr, "CloseComponent err=%ld\n", err);		
		pVdg->seqGrab = NULL;
	}

	if (pVdg) {
		free(pVdg);
		pVdg = NULL;
	}

	return err;
}

#pragma mark -
//  Functions.


int arVideoOpen(char *config)
{
	if (gVid != NULL) {
		fprintf(stderr, "arVideoOpen(): Error, device is already open.\n");
		return (-1);
	}
	gVid = ar2VideoOpen(config);
	if (gVid == NULL) return (-1);

	return (0);
}

int arVideoClose(void)
{
	int result;

	if (gVid == NULL) return (-1);

	result = ar2VideoClose(gVid);
	gVid = NULL;
	return (result);
}  

//ARUint8 *arVideoGetImage(void)
char unsigned* arVideoGetImage(void)
{   
	message("");
	if( gVid == NULL )
	{
		message("");
		return NULL;
	}

	return ar2VideoGetImage( gVid );
}

int arVideoCapStart(void)
{
	if (gVid == NULL) return (-1);

	return (ar2VideoCapStart(gVid));
}  

int arVideoCapStop(void)
{
	if (gVid == NULL) return (-1);

	return (ar2VideoCapStop(gVid));
}       

int arVideoCapNext(void)
{   
	if (gVid == NULL) return (-1);  

	return (ar2VideoCapNext(gVid)); 
}

// function to lock the camera for our own uses.
int ar2VideoInternalLock( pthread_mutex_t *mutex )
{
	int return_val = pthread_mutex_lock( mutex );

	// check that we obtained the lock okay
	if( return_val != 0 )
	{
		// check what the return value means
		if( return_val == EINVAL )
		{
			gen_fatal( "The value specified by mutex is invalid" );
		}
		else if( return_val == EINVAL )
		{
			gen_fatal( "A deadlock would occur if the thread blocked waiting for mutex" );
		}
		else
		{
			gen_fatal( "unspecified error - unable to lock mutex" );
		}
	}

	return return_val;
}

int ar2VideoInternalUnlock(pthread_mutex_t *mutex)
{
	int err;

	// Our access is done, so unlock access to the data.
	if ((err = pthread_mutex_unlock(mutex)) != 0) {
		perror("ar2VideoInternalUnlock(): Error unlocking mutex");
		return (0);
	}
	return (1);
}

void ar2VideoInternalThreadCleanup(void *arg)
{
	AR2VideoParamT *vid;

	vid = (AR2VideoParamT *)arg;
	ar2VideoInternalUnlock(&(vid->bufMutex)); // A cancelled thread shouldn't leave mutexes locked.
	ExitMoviesOnThread();
}

//
// This function will run in a separate pthread.
// Its sole function is to call vdgIdle() on a regular basis during a capture 
// operation. It should be terminated by a call pthread_cancel() from the 
// instantiating thread.
void *ar2VideoInternalThread(void *arg)
{
	OSErr				err_o;
	AR2VideoParamT		*vid;
	int					keepAlive = 1;
	struct timeval	tv;
	struct timespec ts;
	ComponentResult		err;
	int					err_i;
	int					isUpdated = 0;

	char				status[64];
	Str255				theString;
	CGrafPtr			theSavedPort;
	GDHandle			theSavedDevice;

	// Signal to QuickTime that this is a separate thread.
	if ((err_o = EnterMoviesOnThread(0)) != noErr) {
		fprintf(stderr, "ar2VideoInternalThread(): Error %d initing QuickTime for this thread.\n", err_o);
		return (NULL);
	}

	// Register our cleanup function, with arg as arg.
	pthread_cleanup_push(ar2VideoInternalThreadCleanup, arg);

	vid = (AR2VideoParamT *)arg;		// Cast the thread start arg to the correct type.

	// Have to get the lock now, to guarantee vdgIdle() exclusive access to *vid.
	// The lock is released while we are blocked inside pthread_cond_timedwait(),
	// and during that time ar2VideoGetImage() (and therefore OpenGL) can access
	// *vid exclusively.
	if( ar2VideoInternalLock( &( vid->bufMutex ) ) != 0 )
	{
		//	if (!ar2VideoInternalLock(&(vid->bufMutex))) {
		keepAlive = 0;
	}

	while (keepAlive && vdgIsGrabbing(vid->pVdg)) {

		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec;
		ts.tv_nsec = ( long int )( tv.tv_usec * 1000 + vid->milliSecPerTimer * 1E6 );
		if (ts.tv_nsec >= 1E9) {
			ts.tv_nsec -= ( long int )1E9;
			ts.tv_sec += 1;
		}

		if( ( err = vdgIdle( vid->pVdg, &isUpdated ) ) != noErr )
		{
			// In QT 4 you would always encounter a cDepthErr error after a user drags
			// the window, this failure condition has been greatly relaxed in QT 5
			// it may still occur but should only apply to vDigs that really control
			// the screen.
			// You don't always know where these errors originate from, some may come
			// from the VDig.

			message( "vdgIdle err=%ld.", err );
			// ... to fix this we could simply call SGStop and SGStartRecord again
			// calling stop allows the SG to release and re-prepare for grabbing
			// hopefully fixing any problems, this is obviously a very relaxed
			// approach.
			keepAlive = 0;
			break;
		}

		if( isUpdated )
		{
			// Mark status to indicate we have a frame available.
			vid->status |= AR_VIDEO_STATUS_BIT_READY;			
		}

		err_i = pthread_cond_timedwait(&(vid->condition), &(vid->bufMutex), &ts);
		if (err_i != 0 && err_i != ETIMEDOUT) {
			fprintf(stderr, "ar2VideoInternalThread(): Error %d waiting for condition.\n", err_i);
			keepAlive = 0;
			break;
		}

		pthread_testcancel();
	}

	pthread_cleanup_pop(1);
	return (NULL);
	}

#pragma mark -

	int ar2VideoDispOption(void)
	{
		//     0         1         2         3         4         5         6         7
		//     0123456789012345678901234567890123456789012345678901234567890123456789012
		printf("ARVideo may be configured using one or more of the following options,\n");
		printf("separated by a space:\n\n");
		printf(" -nodialog\n");
		printf("    Don't display video settings dialog.\n");
		printf(" -width=w\n");
		printf("    Scale camera native image to width w.\n");
		printf(" -height=h\n");
		printf("    Scale camera native image to height h.\n");
		printf(" -fps\n");
		printf("    Overlay camera frame counter on image.\n");
		printf(" -grabber=n\n");
		printf("    With multiple QuickTime video grabber components installed,\n");
		printf("    use component n (default n=1).\n");
		printf("    N.B. It is NOT necessary to use this option if you have installed\n");
		printf("    more than one video input device (e.g. two cameras) as the default\n");
		printf("    QuickTime grabber can manage multiple video channels.\n");
		printf(" -pixelformat=cccc\n");
		printf("    Return images with pixels in format cccc, where cccc is either a\n");
		printf("    numeric pixel format number or a valid 4-character-code for a\n");
		printf("    pixel format. The following values are supported: \n");
		printf("    32, BGRA, RGBA, ABGR, 24, 24BG, 2vuy, yuvs.\n");
		printf("    (See http://developer.apple.com/quicktime/icefloe/dispatch020.html.)\n");
		printf("\n");

		return (0);
	}



	int ar2VideoCapStart(AR2VideoParamT *vid)
	{
		ComponentResult err;
		int err_i = 0;

		vid->status = 0;

		// check that the digitizer has been preflighted.
		if( !vid->pVdg->isPreflighted )
		{
			// initialise the camera.
			vdgPreflightGrabbing(vid->pVdg);
		}

		if (err_i == 0) {
			if (err = vdgStartGrabbing(vid->pVdg, vid->scaleMatrixPtr)) {
				fprintf(stderr, "ar2VideoCapStart(): vdgStartGrabbing err=%ld\n", err);
				err_i = (int)err;
			}
		}

		if (err_i == 0) {
			// Create the new thread - no attr, vid as user data.
			vid->threadRunning = 1;
			if ((err_i = pthread_create(&(vid->thread), NULL, ar2VideoInternalThread, (void *)vid)) != 0) {
				vid->threadRunning = 0;
				fprintf(stderr, "ar2VideoCapStart(): Error %d detaching thread.\n", err_i);
			}
		}

		return (err_i);
	}

	int ar2VideoCapNext(AR2VideoParamT *vid)
	{
		return (0);
	}

	int ar2VideoCapStop(AR2VideoParamT *vid)
	{
		int err_i = 0;
		void *exit_status_p; // Pointer to return value from thread, will be filled in by pthread_join().
		ComponentResult err = noErr;

		if (vid->threadRunning) {
			// Cancel thread.
			if ((err_i = pthread_cancel(vid->thread)) != 0) {
				fprintf(stderr, "ar2VideoCapStop(): Error %d cancelling ar2VideoInternalThread().\n", err_i);
				return (err_i);
			}

			// Wait for join.
			if ((err_i = pthread_join(vid->thread, &exit_status_p)) != 0) {
				fprintf(stderr, "ar2VideoCapStop(): Error %d waiting for ar2VideoInternalThread() to finish.\n", err_i);
				return (err_i);
			}
			vid->threadRunning = 0;
			vid->thread = NULL;

			// Exit status is ((exit_status_p == AR_PTHREAD_CANCELLED) ? 0 : *(ERROR_t *)(exit_status_p))
		}

		if (vid->pVdg) {
			if ((err = vdgStopGrabbing(vid->pVdg)) != noErr) {
				fprintf(stderr, "vdgStopGrabbing err=%ld\n", err);
				err_i = (int)err;
			}
			vid->status = 0;
			vid->pVdg->isPreflighted = 0;
		}

		return (err_i);
	}

	unsigned char *ar2VideoGetImage(AR2VideoParamT *vid)
	{
		unsigned char *pix = NULL;

		// do some error checking
		if( vid == NULL )
		{
			gen_fatal( "vid was NULL - I don't know why it just is" );
		}

		// Need lock to guarantee this thread exclusive access to vid.
		if( ar2VideoInternalLock( &( vid->bufMutex ) ) != 0 )
		{
			return NULL;
		}

		// ar2VideoGetImage() used to block waiting for a frame. This locked 
		// the OpenGL frame rate to the camera frame rate. Now, if no frame is 
		// currently available then we won't wait around for one. So, do we have 
		// a new frame from the sequence grabber?	
		//	if ( vid->status & AR_VIDEO_STATUS_BIT_READY )
		//	{
		// Prior Mac versions of ar2VideoInternal added 1 to the pixmap base address
		// returned to the caller to cope with the fact that neither
		// arDetectMarker() or argDispImage() knew how to cope with
		// pixel data with ARGB (Apple) or ABGR (SGI) byte ordering.
		// Adding 1 had the effect of passing a pointer to the first byte
		// of non-alpha data. This was an awful hack which caused all sorts
		// of problems and which can now be avoided after rewriting the
		// various bits of the toolkit to cope.
#ifdef AR_VIDEO_DEBUG_BUFFERCOPY
		if (vid->status & AR_VIDEO_STATUS_BIT_BUFFER)
		{
			memcpy((void *)(vid->bufPixelsCopy2), (void *)(vid->bufPixels), vid->bufSize);
			pix = vid->bufPixelsCopy2;
			vid->status &= ~AR_VIDEO_STATUS_BIT_BUFFER; // Clear buffer bit.
		}
		else
		{
			memcpy((void *)(vid->bufPixelsCopy1), (void *)(vid->bufPixels), vid->bufSize);
			pix = vid->bufPixelsCopy1;
			vid->status |= AR_VIDEO_STATUS_BIT_BUFFER; // Set buffer bit.
		}
#else
		pix = vid->bufPixels;
#endif // AR_VIDEO_DEBUG_BUFFERCOPY

		vid->status &= ~AR_VIDEO_STATUS_BIT_READY; // Clear ready bit.
		//	}

		// attempt to release the lock on the camera.	
		if ( !ar2VideoInternalUnlock( &( vid->bufMutex ) ) )
		{
			message( "ar2VideoGetImage(): Unable to unlock mutex." );
			return (NULL);
		}

		return (pix);
	}
