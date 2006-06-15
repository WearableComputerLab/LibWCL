#ifndef __CFOX__DEVICE__CONTROL_DOT_H__INCLUDED__
#define __CFOX__DEVICE__CONTROL_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
  @header     Control
  @abstract   Template class for Firewire Camera control
  @discussion Control is a template that encapsulates all the needed 
  initialisation, interaction and clean-up for one given camera service
  on the Firewire bus. It should never be instanciated by client but
  only by the ControlFactory class.
    
  @author Original Code Sample from Erik Staats, NWG.
          FWCamAkiz was written by Adrian Daerr
          C+FOX was written by Joel Falcou

  Copyright (c) 2002 Adrian Daer & Université Paris VII "Denis Diderot".									 
  Copyright (c) 2004 Joel Falcou & Université Clermont-Ferrand II "Blaise Pascal".									 
  All rights reserved.																						 
                                                                   
  This file is part of the C+FOX Library.  This
  library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1,
  or (at your option) any later version.
                                               
  This library is distributed in the hope that it will be useful,	 
  but WITHOUT ANY WARRANTY; without even the implied warranty of	 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		 
  GNU Lesser General Public License for more details.							 
                                                                   
  You should have received a copy of the GNU Lesser General				 
  Public License along with this library; see the file COPYING.		 
  If not, send mail to the developers of C+FOX										 
                                                                   
  As a special exception, you may use this file as part of a free	 
  software library without restriction. Specifically, if other		 
  files instantiate templates or use macros or inline functions		 
  from this file, or you compile this file and link it with other	 
  files to produce an executable, this file does not by itself		 
  cause the resulting executable to be covered by the GNU Lesser	 
  General Public License.  This exception does not however				 
  invalidate any other reasons why the executable file might be		 
  covered by the GNU Lesser General Public License.
*/
////////////////////////////////////////////////////////////////////////////////

#include <IOKit/firewire/IOFireWireLibIsoch.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/ppc/kern_return.h>
#include <mach/mach_error.h>
#include <unistd.h>
#include <sys/time.h>

#include "formats.h"
#include "constants.h"
#include "exception.h"
#include "FWController.h"
#include "../debug.h"

/************************************************************************/
/*  Some ugly macros I need to keep for some time. 
    TODO : Export those to correctly typed function.
*/
/************************************************************************/

#define ReadFWReg(register) address = FWAddress(0xFFFF, register) ; \
  (*mAsyncReadCommandObjectRef)->SetTarget( (IOFireWireLibCommandRef) mAsyncReadCommandObjectRef, & address); \
  usleep(100000); \
  result = (*mAsyncReadCommandObjectRef)->Submit( (IOFireWireLibCommandRef)mAsyncReadCommandObjectRef)

#define WriteFWReg(register) address = FWAddress( 0xFFFF, register ); \
  ( *mAsyncWriteCommandObjectRef )->SetTarget( ( IOFireWireLibCommandRef )mAsyncWriteCommandObjectRef, &address ); \
  ( *mAsyncWriteCommandObjectRef )->SetFlags( ( IOFireWireLibCommandRef )mAsyncWriteCommandObjectRef, kFWCommandInterfaceSyncExecute ); \
  usleep(100000); \
  result = ( *mAsyncWriteCommandObjectRef )->Submit( ( IOFireWireLibCommandRef )mAsyncWriteCommandObjectRef )

/************************************************************************/
/*
    C+FOX namespace
*/
/************************************************************************/
namespace cfox
{
  /************************************************************************/
  /*!
  @class      Control
  @abstract   Main firewire Device controller
  @discussion Control class is a class designed to select and
  initiliaze a given Firewire device registered through an io
  */
  /************************************************************************/
  class Control
  {
    public:

    /*************************************************************************
    * Control constants neede by DCL setup.
    ************************************************************************/      
    static const int kNumFrameBufs           = 3;
    static const int kLogMaxPacketsPerFrame  = 12;
    static const int kMaxPacketsPerFrame     = ( 1 << kLogMaxPacketsPerFrame ) - 1;
    static const int kMaxTransferDCLs        = kMaxPacketsPerFrame;
    static const int kDCLProgramSize         = (sizeof(DCLTransferPacket) + sizeof(DCLJump) + sizeof(DCLLabel))*(kMaxTransferDCLs + 40);

    /*************************************************************************
    * Public methods
    ************************************************************************/
    public:
                Control();
    virtual    ~Control();

    /*************************************************************************
    *  Main user interface
    ************************************************************************/
    virtual int     toggleCapture( bool state = true);
    virtual int     getFrame( int mode, int frame_no, long time, unsigned char* pixels);
    virtual int     init( bool isTalking = true, bool isConformant = true, size_t idx = 0 );
    virtual void    setVideoFormat(int mode, int format, int framerate) { mVideoFormat=format; mVideoMode=mode; mVideoFrameRate=framerate; }

    virtual int     getVideoFormat()        const   { return mVideoFormat;     }
    virtual int     getVideoMode()          const   { return mVideoMode;       }
    virtual int     getVideoLeft()          const   { return mRoiLeft;         }
    virtual int     getVideoTop()           const   { return mRoiTop;          }
    virtual int     getVideoWidth()         const   { return mRoiWidth;        }
    virtual int     getVideoHeight()        const   { return mRoiHeight;       }
    virtual int     getVideoMaxWidth()      const   { return mMaxWidth;        }
    virtual int     getVideoMaxHeight()     const   { return mMaxHeight;       }
    virtual int     getFrameSize()          const   { return mFrameSize;       }
    virtual double  getFramesPerSecond()    const   { return mFramesPerSecond; }

    /*************************************************************************
    * Static notification handlers used has callback for IOKit functions
    ************************************************************************/
    static void     CaptureIsochHandler( DCLCommandPtr pDCLCommandPtr);
    static OSStatus BusManagementNotify( IOFireWireLibDeviceRef interface, FWClientCommandID commandID );  
    static IOReturn InitIsochPort( IOFireWireLibIsochPortRef interface, IOFWSpeed inSpeed, UInt32 inChannelNum );
    static IOReturn IsochPortGetSupported( IOFireWireLibIsochPortRef interface,IOFWSpeed* outMaxSpeed, UInt64* outChanSupported);
    static IOReturn ReleaseIsochPort( IOFireWireLibIsochPortRef interface);
    static IOReturn StartIsochPort( IOFireWireLibIsochPortRef interface);
    static IOReturn StopIsochPort( IOFireWireLibIsochPortRef interface);
    static void     ClientCommandCompletionProc( void* refCon, IOReturn completionStatus ) {}
    static void     RunawayIsochHandler( DCLCommandPtr ) { std::cout << "RUNAWAY *!*" << std::endl; }

    /*************************************************************************
    *  Internal method used to keep things up
    ************************************************************************/
    protected :

    int         setupFWCamera();
    void        writeDCLProgram() ;
    IOReturn    setupDCLProgram(DCLCallCommandProcPtr callProc) ;
    void        setDCLProgramEndFrame(UInt32 endFrameNum) ;
    void        initAsynchron(IOFireWireLibDeviceRef mInterface );
    void        enableMode7();
    UInt32      readFWRegister(int adr);
    void        writeFWRegister(int adr, UInt32 val);

    /*************************************************************************
    * Internal data
    ************************************************************************/
    private :

    // Frame buffer related
    unsigned char*      mFrameBuf[kNumFrameBufs];   // Buffers used to hold received packets.
    size_t              mFrameBufSize;              // calculated when camera specs known
    IOByteCount         mPacketSize;                // can be chosen between Min and Max
    size_t              mNumPacketsPerFrame;        // depends on ROI size and BYTE_PER_PACKET
    vm_address_t        mVMFrameBuf;                // Virtual memory address of a full(all) frame buffer
    vm_size_t           totalFrameBufferSize;       // Full size of all frame buffers

    // Hardware interface related
    IOFireWireLibDeviceRef          mInterface ;        // FireWire device interface
    IOFireWireLibIsochChannelRef    mIsochChannelRef;   // ID of isochronous channel allocated for camera.
    IOFireWireLibLocalIsochPortRef  mIsochPortRef;      // Local isochronous port ID.
    IOFireWireLibRemoteIsochPortRef mTalkerPortRef;
    size_t                          mIsochChannelNum;   // Above isochronous channel nubmer.
    size_t                          mChannel;           // Requested Channel

    // Command objects for sending asynch (write|read|start|stop) commands to camera.
    IOFireWireLibWriteCommandRef    mAsyncWriteCommandObjectRef;
    IOFireWireLibReadCommandRef     mAsyncReadCommandObjectRef;
    IOFireWireLibWriteCommandRef    mStartAsyncCommandObjectRef;
    IOFireWireLibWriteCommandRef    mStopAsyncCommandObjectRef;      
    IOFireWireLibCommandRef         mIsochPortCommandObjectRef; // Command object for sending isoch port commands.
    size_t                          mStartStopIsochChannelCommandObjectsInUse;  
    size_t                          mFWCCMReg;                  // Buffer to use for asynch requests.   

    // Control state
    bool        mChannelInitialized;    // True if channel has been initialized.
    bool        mChannelActive;         // True if channel is supposed to be active.
    bool        mAsyncMode;             // True if setup buffers has been called.
    bool        mTalker;
    Point       mCurrentLocation;       // Current location in dest pix map.
    size_t      mRegBaseAddress;        // Register file base address offset into initial units space.
    size_t      mReg70BaseAddress;      // Format 7.0 CSR base address offset into initial units space.
    IOByteCount mMinPacketSize;         // depends on camera / video mode / ROI
    IOByteCount mMaxPacketSize;         // depends on camera / video mode / ROI
    IOByteCount mMaxMaxPacketSize;      // Channel limit, set to MaxPacketSize at MaxROI
    UInt64      mSupportedChannelNum;
    IOFWSpeed   mSpeed;
      
    // Frame synchronization handling
    size_t  mNextBufferNum;         // Next image buffer to grab from.
    size_t  mPendingFrameNum;       // First complete received frame
    size_t  mPendingFrameCount;     // Number of complete frames
    size_t  mPendingFrameFragments; // Fragments left in current frame
    size_t  mSlowFrameCounter;      // Times we took too long
    size_t  mSkipFrameCounter;      // Number to skip to recover

    // DCL related
    IOFireWireLibDCLCommandPoolRef  mDCLCommandPool;
    size_t                          mDCLTransferListSize;
    IOByteCount                     mDCLTransferSize[32];                   // max packet size for given DCL command
    DCLCommandStruct*               mDCLList;                               // DCLs used for isoch frame grabs.
    DCLCommandStruct**              mUpdateDCLList;                         // List of DCL's that need updating.
    DCLLabelStruct*                 mPreFrameDCLLabel[kNumFrameBufs];       // Label before transfer DCLs
    DCLTransferPacketStruct*        mFirstDCLTransferPacket[kNumFrameBufs]; // First transfer DCL
    DCLJumpStruct*                  mPostFrameDCLJump[kNumFrameBufs];       // Jump after transfer DCLs
    DCLCallProcStruct*              mPostFrameDCLCallProc[kNumFrameBufs];   // CallProc after jump
    DCLLabelStruct*                 mPostFrameDCLLabel[kNumFrameBufs];      // Label after jump
    DCLLabelStruct*                 mRunawayDCLLabel;                       // Label for runaway condition
    DCLJumpStruct*                  mIntoBlocksDCLJump[kNumFrameBufs];      // into block list
    DCLLabelStruct*                 mOutOfBlocksDCLLabel[kNumFrameBufs];    // block list end
    DCLJumpStruct*                  mBlockDCLJump[kNumFrameBufs][32];       // Jump within frame
    DCLLabelStruct*                 mBlockDCLLabel[kNumFrameBufs][32];

    // Camera settings
    UInt64*                 mBufferTime;        // Table to hold timestamps.
    int                     mRoiLeft;           // ROI left
    int                     mRoiTop;            // ROI top
    int                     mRoiWidth;          // ROI width
    int                     mRoiHeight;         // ROI height
    int                     mMaxWidth;          // image max width
    int                     mMaxHeight;         // image max height
    int                     mVideoFormat;       // video format
    int                     mVideoMode;         // video mode
    int                     mVideoFrameRate;    // video framerate code
    int                     mFrameSize;         // Size of ROI in bytes
    double                  mFramesPerSecond;   // frame rate

    int Q_nextIn;
    int Q_nextGrab;
    int Q_nextFeedback;
    int triggerState;
    int fbCounter;
    int fbFrame_len;
    int fbTime_len;
    int releaseThisBuffer;
    int clearQueueFlag;
    int mode;

    int              QMode[QUEUE_MAX];
    int              QFrameWanted[QUEUE_MAX]; 
    int              QFrameReal[QUEUE_MAX];
    u_int64_t        QTimeWanted[QUEUE_MAX];
    u_int64_t        QTimeReal[QUEUE_MAX];
    unsigned char*   QBuffer[QUEUE_MAX];
  };
}

#endif
