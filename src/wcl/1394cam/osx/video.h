#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#include <pthread.h>
#include <sys/time.h>
//#include "ARVideoSettingsController.h"

#define arMalloc(V,T,S)  \
{ if( ((V) = (T *)malloc( sizeof(T) * (S) )) == 0 ) \
{printf("malloc error!!\n"); exit(1);} }

#define AR_VIDEO_DEBUG_BUFFERCOPY                                       // Uncomment to have ar2VideoGetImage() return a copy of video pixel data.

struct _VdigGrab
{
        // State
        int                                     isPreflighted;
        int                                     isGrabbing;
        int                                     isRecording;

        // QT Components
        SeqGrabComponent        seqGrab;
        SGChannel                       sgchanVideo;
        ComponentInstance   vdCompInst;

        // Device settings
        ImageDescriptionHandle  vdImageDesc;
        Rect                                    vdDigitizerRect;

        // Destination Settings
        CGrafPtr                                dstPort;
        ImageSequence                   dstImageSeq;

        // Compression settings
        short                           cpDepth;
        CompressorComponent cpCompressor;
        CodecQ                          cpSpatialQuality;
        CodecQ                          cpTemporalQuality;
        long                            cpKeyFrameRate;
        Fixed                           cpFrameRate;
};
typedef struct _VdigGrab VdigGrab;
typedef struct _VdigGrab *VdigGrabRef;

struct _AR2VideoParamT {
    int                                         width;
    int                                         height;
    Rect                                        theRect;
    GWorldPtr                           pGWorld;
    int                                         status;
        int                                             showFPS;
        TimeValue                               lastTime;
        long                                    frameCount;
        TimeScale                               timeScale;
        pthread_t                               thread;                 // PRL.
        pthread_mutex_t                 bufMutex;               // PRL.
        pthread_cond_t                  condition;              // PRL.
        int                                             threadRunning;  // PRL.
        long                                    rowBytes;               // PRL.
        long                                    bufSize;                // PRL.
        //ARUint8*                              bufPixels;              // PRL.
        unsigned char*                          bufPixels;              // PRL.
#ifdef AR_VIDEO_DEBUG_BUFFERCOPY
        unsigned char*                          bufPixelsCopy1; // PRL.
//      ARUint8*                                bufPixelsCopy1; // PRL.
//      ARUint8*                                bufPixelsCopy2; // PRL.
        unsigned char*                          bufPixelsCopy2; // PRL.
#endif // AR_VIDEO_DEBUG_BUFFERCOPY
        int                                             grabber;
        MatrixRecordPtr                 scaleMatrixPtr; // PRL.
        VdigGrabRef                             pVdg;                   // DH (see
        long                                    milliSecPerTimer; // DH (seeSaw).
        long                                    milliSecPerFrame; // DH (seeSaw).
        Fixed                                   frameRate;              // DH (see
        long                                    bytesPerSecond; // DH (seeSaw).
        ImageDescriptionHandle  vdImageDesc;    // DH (seeSaw).
};
typedef struct _AR2VideoParamT* AR2VideoParamTRef;
typedef struct _AR2VideoParamT AR2VideoParamT;

 //AR2VideoParamT   *gVid = NULL;
 //unsigned int             gVidCount = 0;


#ifdef __cplusplus
extern "C" {
#endif

OSStatus RequestSGSettings(const int inputIndex, SeqGrabComponent seqGrab, SGChannel sgchanVideo, const int showDialog);

#ifdef __cplusplus
}
#endif

AR2VideoParamT* ar2VideoOpen(char *config);
int ar2VideoDispOption(void);
VdigGrabRef vdgAllocAndInit(const int grabber);
 ComponentResult vdgRequestSettings(VdigGrab* pVdg, const int showDialog, const int inputIndex);
 VideoDigitizerError vdgPreflightGrabbing(VdigGrab* pVdg);
 VideoDigitizerError vdgGetDataRate( VdigGrab*   pVdg, long*           pMilliSecPerFrame, Fixed*      pFramesPerSecond, long*       pBytesPerSecond); 
 VideoDigitizerError vdgGetImageDescription( VdigGrab* pVdg, ImageDescriptionHandle vdImageDesc );
 OSErr vdgSetDestination( VdigGrab* pVdg, CGrafPtr  dstPort );
 ComponentResult vdgReleaseAndDealloc(VdigGrab* pVdg);
int ar2VideoInqSize(AR2VideoParamT *vid, int *x,int *y);
unsigned char *ar2VideoGetImage(AR2VideoParamT *vid);
int ar2VideoCapStart(AR2VideoParamT *vid);
int ar2VideoCapStop(AR2VideoParamT *vid);
int ar2VideoCapNext(AR2VideoParamT *vid);
//OSStatus RequestSGSettings(const int inputIndex, SeqGrabComponent seqGrab, SGChannel sgchanVideo, const int showDialog);

#endif // _VIDEO_H_
