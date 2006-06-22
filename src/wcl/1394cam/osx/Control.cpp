/************************************************************************/
/*!
    @author Original Code Sample from Erik Staats, NWG.
            FWCamAkiz was written by Adrian Daerr

    Copyright (c) 2002 Adrian Daer & Université Paris VII "Denis Diderot" (France).									 
    Copyright (c) 2004 Joel Falcou & Université Clermont-Ferrand II "Blaise Pascal" (France).									 
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
/************************************************************************/

#include "Control.h"

using namespace cfox;

/************************************************************************/
/*!
    @function   stopwatch_us
    @abstract   Timer function
*/
/************************************************************************/
static u_int64_t stopwatch_us(int reset) 
{ 
    static struct timeval time;
    static u_int64_t ms;
    static u_int64_t t0=0;

    gettimeofday(&time,NULL);
    if( reset ) 
    {
        t0=(u_int64_t)1000000*time.tv_sec+(u_int64_t)time.tv_usec;
        return t0;
    }
    else
    {
        ms=(u_int64_t)1000000*time.tv_sec+(u_int64_t)time.tv_usec-t0;
        return ms;
    }
}

/************************************************************************/
/*!
    @method     Control
    @abstract   Control base constructor.
*/
/************************************************************************/
Control::Control() : mFrameBufSize(0),mPacketSize(0),mNumPacketsPerFrame(0), mVMFrameBuf(0),totalFrameBufferSize(0),
                     mInterface(0),mIsochChannelRef(0),mIsochPortRef(0),mTalkerPortRef(0),mIsochChannelNum(0),mChannel(0),
                     mAsyncWriteCommandObjectRef(0),mAsyncReadCommandObjectRef(0), mStartAsyncCommandObjectRef(0),
                     mStopAsyncCommandObjectRef(0),mIsochPortCommandObjectRef(0), mStartStopIsochChannelCommandObjectsInUse(0),
                     mFWCCMReg(0),mChannelInitialized(false),mChannelActive(false),mAsyncMode(false),mTalker(false),
                     mSkipFrameCounter(0),mDCLCommandPool(0),mDCLList(0), mUpdateDCLList(0),mBufferTime(nil),Q_nextIn(0),Q_nextGrab(0),
                     Q_nextFeedback(0), triggerState(0),fbCounter(0),fbFrame_len(0),fbTime_len(0), releaseThisBuffer(0),clearQueueFlag(0),
                     mode(0)
{
	message( "kNumFrameBufs = %d", kNumFrameBufs );

    for( int i=0;i < kNumFrameBufs;i++ ) mFrameBuf[i] = nil ;
}

/************************************************************************/
/*!
    @method     ~Control
    @abstract   Control base destructor.
*/
/************************************************************************/
Control::~Control()
{
    if(mTalker)
    {
        if( mIsochChannelRef && mChannelInitialized )
        {
            (*mIsochChannelRef)->Stop(mIsochChannelRef);
            (*mIsochChannelRef)->ReleaseChannel(mIsochChannelRef);    
        } 
           
        // Deallocate FireWire command objects.
        if( mAsyncWriteCommandObjectRef ) (*mAsyncWriteCommandObjectRef)->Release(mAsyncWriteCommandObjectRef);
        if( mAsyncReadCommandObjectRef  ) (*mAsyncReadCommandObjectRef)->Release(mAsyncReadCommandObjectRef);
        if( mStartAsyncCommandObjectRef ) (*mStartAsyncCommandObjectRef)->Release(mStartAsyncCommandObjectRef);
        if( mStopAsyncCommandObjectRef  ) (*mStopAsyncCommandObjectRef)->Release(mStopAsyncCommandObjectRef);

        // Deallocate isochronous channel ID for receiving video data.
        if (mIsochChannelRef) (*mIsochChannelRef)->Release(mIsochChannelRef);
    }
  

    if( mVMFrameBuf != nil ) vm_deallocate(mach_task_self(), mVMFrameBuf, totalFrameBufferSize);
    delete[] mBufferTime;

    if(mTalker)
    {
        FWController* fwc = FWController::Instance();
        fwc->releaseService(mChannel);
    }
}

/************************************************************************/
/*!
    @method     ReadFWRegister
    @abstract   Read a FW Device Register
*/
/************************************************************************/
UInt32 Control::readFWRegister(int reg) 
{
    FWAddress address;
    int result = noErr;
    ReadFWReg(mRegBaseAddress + reg);
    return mFWCCMReg;
}

/************************************************************************/
/*!
    @method     WriteFWRegister
    @abstract   Write to a FW Device Register
*/
/************************************************************************/
void Control::writeFWRegister(int reg, UInt32 value) 
{
    FWAddress address;
    int result = noErr;
    mFWCCMReg  = value;
    WriteFWReg(mRegBaseAddress + reg);

	if( result != noErr )
	{
		gen_fatal( "writeFWRegister failed" );
	}
}

/************************************************************************/
/*!
    @method     toggleCapture
    @abstract   Control continuous digitization.
*/
/************************************************************************/
int Control::toggleCapture(bool state)
{
    if( mChannelInitialized )
    {
        mChannelActive = false ;
        (*mIsochChannelRef)->Stop(mIsochChannelRef) ;
        (*mIsochChannelRef)->ReleaseChannel(mIsochChannelRef) ;
    }

    mChannelInitialized = false ;

    if( state )
    {
        setupDCLProgram( &Control::CaptureIsochHandler ) ;
        mChannelActive          = true;
        mChannelInitialized     = true;
        mCurrentLocation.v      = 0;
        mCurrentLocation.h      = 0;
        (*mIsochChannelRef)->AllocateChannel(mIsochChannelRef) ;      
        (*mIsochChannelRef)->Start(mIsochChannelRef) ;
    }

    return noErr;
}

/************************************************************************/
/*!
    @method     setupFWCamera
    @abstract   Initializes the camera for a given mode,format and fps.
*/
/************************************************************************/
int Control::setupFWCamera() 
{
    FWAddress  address;
    int result = noErr;
    
	message( "in setupFWCamera" );

    if( mTalker )
    {	
	message( "About to tell the firewire camera what video mode to use" );
        mFWCCMReg = mVideoMode << 29;
        WriteFWReg(mRegBaseAddress + 0x0608);

	// check that the write went okay
	if( result != noErr )
	{
		gen_fatal( "couldn't set video mode" );
	}
	
	// check the status of the last write
	result = ( *mAsyncWriteCommandObjectRef )->GetStatus( ( IOFireWireLibCommandRef )mAsyncWriteCommandObjectRef );

	// check that the write went okay
	if( result != noErr )
	{
		gen_fatal( "the status was not good" );
	}
        
	message( "About to tell the firewire camera what video format to use" );
	message( "mVideoFormat = %d", mVideoFormat );
        mFWCCMReg = mVideoFormat << 29;
        WriteFWReg(mRegBaseAddress + 0x0604);
        
	// check that the write went okay
	if( result != noErr )
	{
		gen_fatal( "couldn't set video format" );
	}

	// check the status of the last write
	result = ( *mAsyncWriteCommandObjectRef )->GetStatus( ( IOFireWireLibCommandRef )mAsyncWriteCommandObjectRef );

	// check that the write went okay
	if( result != noErr )
	{
		gen_fatal( "the status was not good" );
	}

        mFWCCMReg = mVideoFrameRate << 29;
        WriteFWReg(mRegBaseAddress + 0x0600);
       
	// check that the write went okay
	if( result != noErr )
	{
		gen_fatal( "couldn't set frame rate" );
	}

	// check the status of the last write
	result = ( *mAsyncWriteCommandObjectRef )->GetStatus( ( IOFireWireLibCommandRef )mAsyncWriteCommandObjectRef );

	// check that the write went okay
	if( result != noErr )
	{
		gen_fatal( "the status was not good" );
	}

        mFWCCMReg = 0;
        WriteFWReg(mRegBaseAddress + 0x0830);
    }
    
    return result;
}

/************************************************************************/
/*!
    @method     Init
    @abstract   Initialize the camera settings.
    @discussion Handles all the needed steps to get the camera working
    in a specified mode, format and framerate. Also take care of specious
    camera settings and FWBroadcast support.
*/
/************************************************************************/
int Control::init( bool isTalking, bool isConformant, size_t idx )
{
	ComponentResult   result = noErr;
	FWAddress         address;
	FWController*     fwc = FWController::Instance();

	mTalker = isTalking;
	mChannel = idx;

	// ----------------------------------------------------------
	// Acquire FW interface for a given camera on the channel
	// ----------------------------------------------------------
	// check to make sure that fwc objects was instantiated.
	if( fwc == NULL )
	{
		gen_fatal( "firewire controller is NULL, is the camera plugged in?" );
	}
	
	mInterface = fwc->acquireService(mChannel,mDCLCommandPool,mUpdateDCLList);

	message( "attempting to get unit info" );
	fwc->getUnitInfo( mChannel, mRegBaseAddress );

	// ----------------------------------------------------------
	// Prepare asynchronous transmition
	// ----------------------------------------------------------
	message( "preparing asynchronous transmission" );
	initAsynchron( mInterface );

	// ----------------------------------------------------------
	// Prepare for conformant camera
	// ----------------------------------------------------------
	message( "prepare for conformant camera" );
	if( isConformant )
	{
		mFWCCMReg = 0x80000000;
		WriteFWReg(mRegBaseAddress + 0x0000);
	}

	// ----------------------------------------------------------
	// Select video frame size from format & mode
	// ----------------------------------------------------------
	message( "select video frame size from format and mode" );
	if (mVideoMode != 7) 
	{
		mMaxWidth  = Width[mVideoMode][mVideoFormat];
		mMaxHeight = Height[mVideoMode][mVideoFormat];
		mMaxMaxPacketSize = PacketByteSize[mVideoMode][mVideoFormat][mVideoFrameRate]; 

		mRoiLeft   = 0;
		mRoiTop    = 0;
		mRoiWidth  = mMaxWidth;
		mRoiHeight = mMaxHeight;

		// Here we assume a 8bpp depth.
		// TODO : handle color camera with RGB triplets.
		mFrameSize = mRoiWidth * mRoiHeight * VideoModeSize[mVideoMode][mVideoFormat];
	}
	else 
	{
		enableMode7();
	}

	mMaxPacketSize      = mMaxMaxPacketSize;
	mPacketSize         = mMaxPacketSize;
	mNumPacketsPerFrame = (mRoiWidth*mRoiHeight*VideoModeSize[mVideoMode][mVideoFormat]+mPacketSize-1) / mPacketSize;

	if( mNumPacketsPerFrame > size_t(kMaxPacketsPerFrame) )
	{
		gen_fatal( "Invalid number of packets per frame" );
		//throw InvalidPacketsPerFrame();
	}

	if (mVideoMode != 7) 
	{
		mFramesPerSecond = (1 << mVideoFrameRate)*1.875;
	} 
	else 
	{
		mFramesPerSecond = 8000.0/mNumPacketsPerFrame;
	}

	// -------------------------------------
	// Allocate frame and timestamp buffers.
	// -------------------------------------
	message( "allocate frame and timestamp buffers" );
	mFrameBufSize        = mFrameSize + sizeof(UInt32)*kMaxPacketsPerFrame;
	totalFrameBufferSize = kNumFrameBufs * mFrameBufSize;

	result = vm_allocate( mach_task_self(), &mVMFrameBuf, totalFrameBufferSize, TRUE );

	if (result == noErr) 
	{
		for( int i = 0; i < kNumFrameBufs; i++ ) 
		{
			if (nil == (mFrameBuf[i] = (UInt8*)(mVMFrameBuf + (mFrameBufSize*i)) )) result = memFullErr ;
		}
	}

	if (result == memFullErr)
	{
		result = vm_deallocate(mach_task_self(), mVMFrameBuf, totalFrameBufferSize);
		gen_fatal( "not enough memory" );
		//throw NotEnoughMemory();
	} 
	else if (result != noErr) 
	{
		gen_fatal( "Frame buffer allocation failure" );
		//throw FrameBufferAllocationFailure();
	}

	// ----------------------------------
	// Allocate buffer for timestamps
	// ----------------------------------
	message( "allocate buffer for timestamps" );
	mBufferTime = new UInt64[kNumFrameBufs]; 
	if( mBufferTime == nil )
	{
		gen_fatal( "Not enough memory" );
		// throw NotEnoughMemory();
	}
	// ----------------------------------
	// Write general-purpose DCL program.
	// ----------------------------------
	message( "write general purpose DCL program" );
	writeDCLProgram();

	// ------------------------------------------------------
	// Allocate an isochronous channel to receive video data.
	// ------------------------------------------------------
	message( "allocate an isochronous channel to receive video data" );
	mIsochChannelRef = (*mInterface)->CreateIsochChannel( mInterface,isTalking,mMaxMaxPacketSize,
			kFWSpeed400MBit, CFUUIDGetUUIDBytes(kIOFireWireIsochChannelInterfaceID) );

	if (nil == mIsochChannelRef)
	{
		gen_fatal( "Isoch channel failed." );
		 //throw IsochChannelFailed();
	}
	mTalkerPortRef = (*mInterface)->CreateRemoteIsochPort(mInterface, true, CFUUIDGetUUIDBytes(kIOFireWireRemoteIsochPortInterfaceID));
	if (!mTalkerPortRef)
	{
		gen_fatal( "not enough memory" );
	 	//throw NotEnoughMemory();
	}
	(*mTalkerPortRef)->SetRefCon( (IOFireWireLibIsochPortRef) mTalkerPortRef, this ) ;

	if( isTalking )
	{
		(*mTalkerPortRef)->SetAllocatePortHandler(mTalkerPortRef, &Control::InitIsochPort);
		(*mTalkerPortRef)->SetReleasePortHandler(mTalkerPortRef, &Control::ReleaseIsochPort);
		(*mTalkerPortRef)->SetStartHandler(mTalkerPortRef, &Control::StartIsochPort);
		(*mTalkerPortRef)->SetStopHandler(mTalkerPortRef, &Control::StopIsochPort);
	}

	(*mTalkerPortRef)->SetGetSupportedHandler(mTalkerPortRef, &Control::IsochPortGetSupported);

	result = (*mIsochChannelRef)->SetTalker(mIsochChannelRef, (IOFireWireLibIsochPortRef) mTalkerPortRef);

	// -----------------------------------
	// Add local node as listening client.
	// -----------------------------------
	message( "Add local node as listening client" );
	mIsochPortRef = (*mInterface)->CreateLocalIsochPort(mInterface, false /*inTalking*/, mDCLList, 0, 0, 0, nil, 0, nil, 0, CFUUIDGetUUIDBytes(kIOFireWireLocalIsochPortInterfaceID));
	if (!mIsochPortRef)
	{
		gen_fatal( "not enough memory" );
		//throw NotEnoughMemory();
	}

	result = (*mIsochChannelRef)->AddListener(mIsochChannelRef, (IOFireWireLibIsochPortRef) mIsochPortRef);

	mChannelInitialized = true ;
	mChannelActive = false ;
	mAsyncMode = false ;

	return result;
}

/************************************************************************/
/*!
  @method     writeDCLProgram
  @abstract   Initializes the DCL program fro dat atransfer.

  @discussion This routine is called once, each time we are opened.
  It constructs a DCL program suitable for all three kinds of receive 
  (playthrough, sync grab,and async grab).  As the usage changes, only 
  the callProc will be varied (but not while the DCL program actually runs).

  The DCL program consists of consecutive frame-sized sets of receive buffers.
  If we can't keep up with the received data, whole frames will be skipped.
  But, we never skip a frame and then immediately want one that isn't there,
  because we have a total of three frames.

  The high level structure looks like this:
  Receive frame 1, branch to frame 2.
  Receive frame 2, branch to frame 3.
  Receive frame 3, branch to frame 3.

Note:   Frame 3 will be overwritten with frames 4, 5, 6, etc., unless we
change the branch that follows it, before that branch is reached.
Our goal is to do so, unless the CPU can't keep up.

Once frame 1 has been received and processed, and the buffer is again
available, the DMA is running somewhere in frame 2 or 3.  The branches
are then changed so that the high-level structure now looks like this:
Receive frame 1, branch to frame 1.
Receive frame 2, branch to frame 3.
Receive frame 3, branch to frame 1.

If we had only two frame buffers, and the CPU speed varied (due to other loads)
and we had to sometimes skip frames, we might have the following sequence of
events:  We are processing frame 1.  Frame 2 completes, and the DMA starts to
receive frame 3 into the same buffer.  But now we get full use of the CPU for
a while.  We wrap up frame 1, and charge into frame 3 - and we overtake the DMA,
which is still filling in frame 3.  So we glitch the video, because we'll show an
image that is partly frame 3 and partly frame 2.  The three-buffer solution used
here prevents this problem.

The lower level structure of the DCL program for a single frame looks like this:
kDCLLabelOp "preFrame"
kDCLReceivePacketStartOp x mNumPacketsPerFrame
kDCLJumpOp
kDCLLabelOp "postFrame"
kDCLUpdateDCLListOp
kDCLCallProcOp

Usually, the Jump points to the "postFrame" label that follows it, meaning the
jump has no real effect.  But in the "overflow" frame at the end of the DCL program,
the Jump points to the "preFrame" label that heads the frame, making the DMA fill
the same buffer again.  Because the Jump takes place before the update and the
callProc, these tasks can be avoided when a frame of data is overwritten, to
save time (and maybe help catch up).

Note that while the high-level description shows a branch to the next frame, the
actual Jump normally just moves execution forward by one DCL.  After each callProc,
the DCLs for the next frame will begin.  [Except at the end of program memory,
where there is an extra Jump back to the top of the DCL program.]

The use of large, frame-sized blocks of DCLs serves three purposes.  First, it allows
for the preservation of vertical lock in systems such as Pele that cannot detect
the start-of-frame sync pattern in an already-running DMA program.  Second, even for
systems that could detect sync (OpenHCI, Lynx), this DMA arrangement neatly skips
whole frames of data, without forcing the CPU to spend extra time taking callProcs
for data that will not be used.  Finally, the use of three frames means that we can
degrade gracefully when we cannot keep up.  For example, with a two-frame (or smaller)
program, if we can't keep up with 30 fps, we'll end up dropping every other frame
and netting about 15 fps.  The three-frame program, however, can degrade to 29, 28,
or some other fps, because it has more elasticity.

 */
/************************************************************************/
void Control::writeDCLProgram()
{
	typedef void (*f)(DCLCommand*);

	DCLLabelPtr           pPreFrameDCLLabel;
	DCLTransferPacketPtr  pFirstDCLTransferPacket;
	DCLCallProcPtr        pDCLCallProc;
	DCLJumpPtr            pDCLJump;
	DCLCommandStruct**    frameUpdateDCLList ;
	UInt32                updateListSize;
	UInt8*                frameBuffer;

	DCLCommandStruct**  updateDCLList   = mUpdateDCLList ;
	DCLCommandStruct*   pDCLCommand     = nil;

	for( int frameNum = 0; frameNum < kNumFrameBufs; frameNum++)  
	{
		frameBuffer = mFrameBuf[frameNum];
		pDCLCommand = (*mDCLCommandPool)->AllocateLabelDCL(mDCLCommandPool, pDCLCommand);
		pPreFrameDCLLabel = (DCLLabelStruct*)(pDCLCommand);

		if(!frameNum)   mDCLList = (DCLCommandStruct*)(pPreFrameDCLLabel);

		frameUpdateDCLList = updateDCLList;
		updateListSize = 0;

		for( size_t packetNum = 0; packetNum < mNumPacketsPerFrame; packetNum++ )
		{
			pDCLCommand = (*mDCLCommandPool)->AllocateReceivePacketStartDCL(mDCLCommandPool,pDCLCommand,frameBuffer,mPacketSize+sizeof(UInt32)) ;
			*updateDCLList++ = pDCLCommand;
			updateListSize++;
			frameBuffer += mPacketSize + sizeof(UInt32);
		}

		pFirstDCLTransferPacket = (DCLTransferPacketPtr) *frameUpdateDCLList;
		pDCLCommand = (*mDCLCommandPool)->AllocateUpdateDCLListDCL(mDCLCommandPool,pDCLCommand, frameUpdateDCLList, updateListSize);

		pDCLCommand = (*mDCLCommandPool)->AllocateCallProcDCL(mDCLCommandPool,pDCLCommand, nil, (UInt32) this);
		pDCLCallProc = (DCLCallProcStruct*) pDCLCommand;
		pDCLCallProc->proc    = &Control::CaptureIsochHandler;

		pDCLCommand = (*mDCLCommandPool)->AllocateJumpDCL(mDCLCommandPool,pDCLCommand, pPreFrameDCLLabel);
		pDCLCommand->opcode |= kFWDCLOpDynamicFlag;
		pDCLJump    = (DCLJumpStruct*) pDCLCommand;

		mPreFrameDCLLabel[frameNum]       = pPreFrameDCLLabel;
		mFirstDCLTransferPacket[frameNum] = pFirstDCLTransferPacket;
		mPostFrameDCLJump[frameNum]       = pDCLJump;
		mPostFrameDCLCallProc[frameNum]   = pDCLCallProc;
	}

	pDCLCommand         = (*mDCLCommandPool)->AllocateLabelDCL(mDCLCommandPool, pDCLCommand);
	mRunawayDCLLabel    = (DCLLabelStruct*) pDCLCommand;
	frameBuffer         = mFrameBuf[0];

	pDCLCommand         =(*mDCLCommandPool)->AllocateReceivePacketStartDCL(mDCLCommandPool, pDCLCommand,frameBuffer,mPacketSize+sizeof(UInt32));
	pDCLCommand         = (*mDCLCommandPool)->AllocateCallProcDCL(mDCLCommandPool,pDCLCommand, nil, (UInt32) this);
	pDCLCallProc        = (DCLCallProcStruct*) pDCLCommand;
	pDCLCallProc->proc  = & Control::RunawayIsochHandler;

	pDCLCommand->pNextDCLCommand = nil;
}


/************************************************************************/
/*!
  @method     getFrame
  @abstract   request a new frame from stream.
 */
/************************************************************************/
int Control::getFrame( int mode, int frame_no, long time, unsigned char* pixels )
{
	message( "in getFrame" );
	int lenQueue = ( Q_nextIn - Q_nextGrab + QUEUE_MAX ) % QUEUE_MAX;

	if( lenQueue == QUEUE_MAX-1 ) 
	{
		gen_fatal( "lenQueue == QUEUE_MAX" );
		return 1;
	}

	if( pixels == NULL ) 
	{
		gen_fatal( "pixels == NULL" );
		return 2;
	}

	QBuffer[Q_nextIn]      = pixels;
	QFrameWanted[Q_nextIn] = frame_no;
	QTimeWanted[Q_nextIn]  = time;
	QMode[Q_nextIn]        = mode;

	Q_nextIn = (Q_nextIn+1) % QUEUE_MAX;

	message( " end of get frame" );

	return 0;
}

void Control::initAsynchron( IOFireWireLibDeviceRef mInterface )
{

	message( "initializing asynchronized" );
	
	// attempt to make a block write command object. (returns a
	// mca IOFireWireLibCommandRef
	mAsyncWriteCommandObjectRef = 
		(IOFireWireLibWriteCommandRef)((*mInterface)->CreateWriteCommand(
										 mInterface, (*mInterface)->GetDevice(mInterface),
										 nil, nil, 0, nil, kFWDontFailOnReset, 0, this, 
										 CFUUIDGetUUIDBytes(kIOFireWireWriteCommandInterfaceID)));

	// set the buffer where the read data should be stored.
	(*mAsyncWriteCommandObjectRef)->SetBuffer( (IOFireWireLibCommandRef)mAsyncWriteCommandObjectRef, sizeof(mFWCCMReg), &mFWCCMReg );

	mStartAsyncCommandObjectRef = 
		(IOFireWireLibWriteCommandRef)((*mInterface)->CreateWriteCommand( 
										 mInterface, (*mInterface)->GetDevice(mInterface),
										 nil, nil, 0, nil, kFWDontFailOnReset, 0, this, 
										 CFUUIDGetUUIDBytes(kIOFireWireWriteCommandInterfaceID))) ;
	(*mStartAsyncCommandObjectRef)->SetBuffer( (IOFireWireLibCommandRef)mStartAsyncCommandObjectRef, sizeof(mFWCCMReg), &mFWCCMReg );

	mStopAsyncCommandObjectRef = 
		(IOFireWireLibWriteCommandRef)((*mInterface)->CreateWriteCommand( 
										 mInterface, (*mInterface)->GetDevice(mInterface),
										 nil, nil, 0, nil, kFWDontFailOnReset, 0, this, 
										 CFUUIDGetUUIDBytes(kIOFireWireWriteCommandInterfaceID))) ;
	(*mStopAsyncCommandObjectRef)->SetBuffer( (IOFireWireLibCommandRef) mStopAsyncCommandObjectRef, sizeof( mFWCCMReg ), &mFWCCMReg ) ;

	mAsyncReadCommandObjectRef = 
		(IOFireWireLibReadCommandRef)((*mInterface)->CreateReadCommand(
									       mInterface, (*mInterface)->GetDevice(mInterface),
									       nil, nil, 0, nil, kFWDontFailOnReset, 0, this, 
									       CFUUIDGetUUIDBytes(kIOFireWireReadCommandInterfaceID))) ;
	(*mAsyncReadCommandObjectRef)->SetBuffer( 
						 (IOFireWireLibCommandRef) mAsyncReadCommandObjectRef, sizeof( mFWCCMReg ), &mFWCCMReg ) ;

}

void Control::enableMode7()
{
	ComponentResult   result = noErr;
	FWAddress         address;

	// ----------------------------------------------------------
	// Get video format 7 mode 0 CSR base adress
	// ----------------------------------------------------------
	ReadFWReg(mRegBaseAddress+0x02e0);
	mReg70BaseAddress = 0xf0000000 | (mFWCCMReg << 2);

	if( result != noErr )
	{
		gen_fatal( "Mode seven failure" );
		//throw ModeSevenFailure();
	}

	mFWCCMReg = 7 << 29;
	WriteFWReg(mRegBaseAddress + 0x0608);
	mFWCCMReg = 0;
	WriteFWReg(mRegBaseAddress + 0x0604) ;

	if(result != noErr)
	{
		gen_fatal( "mode seven failure" );
		//throw ModeSevenFailure();
	}

	// ----------------------------------------------------------
	// Get video format 7 width and height
	// ----------------------------------------------------------
	ReadFWReg(mReg70BaseAddress + 0x0000);
	mMaxWidth = mFWCCMReg >> 16;
	mMaxHeight = mFWCCMReg & 0xffff;

	// ----------------------------------------------------------
	// Compute correct ROI bounds
	// ----------------------------------------------------------
	if( (mRoiLeft < 0)   || (mRoiTop < 0)     || (mRoiWidth <= 0) || (mRoiHeight <= 0) || 
			( (mRoiLeft+mRoiWidth) > mMaxWidth)   || ( (mRoiTop+mRoiHeight) > mMaxHeight )
	  ) 
	{
		gen_fatal( "invalid ROI" );
		//throw InvalidROI();
	}

	// Here we assume a 8bpp depth.
	// TODO : handle color camera with RGB triplets.
	mFrameSize = mRoiWidth * mRoiHeight*VideoModeSize[mVideoMode][mVideoFormat];

	// ----------------------------------------------------------
	// Set image position to (mRoiLeft,mRoiTop).
	// ----------------------------------------------------------
	mFWCCMReg = (mRoiLeft << 16) | mRoiTop;
	WriteFWReg(mReg70BaseAddress + 0x0008);

	// ----------------------------------------------------------
	// Set image size to mRoiWidth x mRoiHeight.
	// ----------------------------------------------------------
	mFWCCMReg = (mRoiWidth << 16) | mRoiHeight;
	WriteFWReg(mReg70BaseAddress + 0x000c);

	// Get maximum allowed packet size.
	// --------------------------------
	ReadFWReg(mReg70BaseAddress + 0x0040);
	mMaxMaxPacketSize = mFWCCMReg & 0xffff;
	mMinPacketSize = mFWCCMReg >> 16;
}

/************************************************************************
 * Static Callback for Control Class
 ************************************************************************/

/************************************************************************/
/*!
  @function   CaptureIsochHandler
  @abstract   Handle the isochronous buffers for capture.
 */
/************************************************************************/
void Control::CaptureIsochHandler(DCLCommandPtr pDCLCommandPtr  )
{
	DCLCallProcPtr          pDCLCallProc;
	DCLTransferPacketPtr    pDCLCommand;
	UInt32                  packetHeader;
	UInt32                  syncBits;
	UInt32                  packetNum;
	UInt32                  bufferSize;
	AbsoluteTime            startTime;
	AbsoluteTime            endTime;
	AbsoluteTime            runTime;
	Duration                duration;
	size_t                  lenQueue;
	int                     copy_buffer;
	static int              curr_buffer   = 0;
	static int              frame_counter = 0;
	static int              grab_counter  = 0;
	static int              frame_origin  = 0;
	static int              lastGrabFrame = 0;
	static u_int64_t        time_origin   = 0;
	static u_int64_t        lastGrabTime  = 0;
	unsigned long           offset        = 0;
	unsigned long           imageSize;
	IOByteCount             packetSize;
	unsigned char           *pBuffer;
	unsigned char           *pSrc;
	unsigned char           *pDest;

	startTime = UpTime();
	pDCLCallProc = DCLCallProcPtr(pDCLCommandPtr);
	Control* me = (Control*)(pDCLCallProc->procData);
	me->mBufferTime[curr_buffer] = stopwatch_us(0);

	if(!frame_counter) 
	{
		time_origin = stopwatch_us(1);
		time_origin = 0;
		me->mBufferTime[curr_buffer] = 0;
	}

	lenQueue = (me->Q_nextIn-me->Q_nextGrab+QUEUE_MAX) % QUEUE_MAX;

	if(lenQueue > 0) 
	{
		copy_buffer = -1;
		me->mode = me->QMode[me->Q_nextGrab];

		if(me->mode & MODE_FRAME) 
		{
			// transform to world framecount if not done yet
			if(me->mode & MODE_RELATIVE) 
			{
				me->QFrameWanted[me->Q_nextGrab] += lastGrabFrame;
				me->QMode[me->Q_nextGrab]        ^= MODE_RELATIVE;
			}

			if(me->mode & MODE_ABSOLUTE) 
			{
				me->QFrameWanted[me->Q_nextGrab] += frame_origin;
				me->QMode[me->Q_nextGrab]        ^= MODE_ABSOLUTE;
			}

			if(frame_counter >= me->QFrameWanted[me->Q_nextGrab]) 
			{
				// We'll try to grab a frame and we have to decide which one
				if( frame_counter-me->QFrameWanted[me->Q_nextGrab] < kNumFrameBufs-1 ) 
				{
					copy_buffer = (curr_buffer + kNumFrameBufs - frame_counter + me->QFrameWanted[me->Q_nextGrab]) % kNumFrameBufs;
				} 
				// We don't have the right frame any more; take the oldest instead
				else 
				{ 
					copy_buffer = (curr_buffer + 2) % kNumFrameBufs;
				}
			}
		}
		else if(me->mode & MODE_TIME) 
		{
			// transform to world time if not done yet 
			if(me->mode & MODE_RELATIVE) 
			{
				me->QTimeWanted[me->Q_nextGrab] += lastGrabTime;
				me->QMode[me->Q_nextGrab]       ^= MODE_RELATIVE;
			}

			if(me->mode & MODE_ABSOLUTE) 
			{
				me->QTimeWanted[me->Q_nextGrab] += time_origin;
				me->QMode[me->Q_nextGrab]       ^= MODE_ABSOLUTE;
			}

			if(me->mBufferTime[curr_buffer] >= me->QTimeWanted[me->Q_nextGrab]) 
			{
				for( size_t i=0;i<kNumFrameBufs-1;i++ ) 
				{
					copy_buffer = (curr_buffer + kNumFrameBufs - i) % kNumFrameBufs;
					if (me->mBufferTime[(copy_buffer+kNumFrameBufs-1)%kNumFrameBufs] < me->QTimeWanted[me->Q_nextGrab]) break;
				}
			}
		}

		if (copy_buffer >= 0) 
		{
			pDCLCommand = me->mFirstDCLTransferPacket[me->mPendingFrameNum];
			pDest       = me->QBuffer[me->Q_nextGrab];
			offset      = 0;
			imageSize   = me->mRoiWidth*me->mRoiHeight*VideoModeSize[me->mVideoMode][me->mVideoFormat];
			packetSize  = me->mPacketSize;

			for (packetNum = 0; packetNum < me->mNumPacketsPerFrame; packetNum++)
			{
				pBuffer         = (unsigned char*)(pDCLCommand->buffer);
				packetHeader    = *((UInt32 *)(pBuffer));
				syncBits        = (packetHeader & kFWIsochSy) >> kFWIsochSyPhase;

				// Set to top of frame if sync bits indicate this packet as the
				// top of frame (Lynx), or if we've filled up a frame (Lynx/Pele).
				if ((syncBits == 1) || (offset >= imageSize))
				{
					me->mCurrentLocation.v = 0 ;
					me->mCurrentLocation.h = 0 ;
					offset = 0;
				}

				bufferSize=(packetHeader &kFWIsochDataLength)>>kFWIsochDataLengthPhase;

				if(bufferSize > packetSize)             bufferSize = packetSize;
				if((offset+bufferSize) > imageSize)     bufferSize = imageSize - offset;

				pSrc = pBuffer+sizeof(UInt32);

				memcpy(pDest+offset,pSrc,bufferSize);
				offset += bufferSize;

				pDCLCommand = (DCLTransferPacketPtr) pDCLCommand->pNextDCLCommand;
			}

			me->releaseThisBuffer = me->Q_nextGrab;

			me->QTimeReal[me->Q_nextGrab] = me->mBufferTime[copy_buffer];
			lastGrabTime = me->QTimeReal[me->Q_nextGrab];
			me->QFrameReal[me->Q_nextGrab] = frame_counter - (curr_buffer - copy_buffer + kNumFrameBufs) % kNumFrameBufs;
			lastGrabFrame = me->QFrameReal[me->Q_nextGrab];
			me->Q_nextGrab = (me->Q_nextGrab+1) % QUEUE_MAX;
			grab_counter++;

			if (me->mode & MODE_RESET_FRAME) frame_origin = 0;
			if (me->mode & MODE_RESET_TIME) time_origin = me->mBufferTime[curr_buffer];
		}
	}


	int temp = ++curr_buffer; 
	curr_buffer = temp % kNumFrameBufs;
	frame_counter++;

	// We're done with this frame, so make it available at the end of the program.
	me->setDCLProgramEndFrame ( me->mPendingFrameNum );

	endTime = UpTime ();
	runTime = SubAbsoluteFromAbsolute (endTime, startTime);
	duration = AbsoluteToDuration (runTime);

	if(duration < 0)                            duration = duration / -1000;
	if(duration > (durationMillisecond * 37))   me->mSlowFrameCounter++;
	if(duration < (durationMillisecond * 32))   me->mSlowFrameCounter = 0;

	// If we have more than 4 slow frames before a fast one, we are probably
	// saturated (or very close).  To recover, skip the next two frames.  This
	// makes the image stream bursty, but if we skip only one, the DMA may have
	// another frame already waiting for us, and we will not recover.

	if (me->mSlowFrameCounter > 4)
	{
		me->mSlowFrameCounter = 0;
		me->mSkipFrameCounter = 2;
	}

	me->mPendingFrameNum = ( me->mPendingFrameNum + 1 ) % kNumFrameBufs;
}

/************************************************************************/
/*!
  @function   BusManagementNotify
  @abstract   Handle reset notification when bus management has been
  established.  If ISO_EN is set before the cycle master is enabled, ISO_EN
  will get cleared with the cycle master begins sending cycle start packets.
  Thus, we need this routine to execute after the bus manager has enabled
  the cycle master.
 */
/************************************************************************/
OSStatus Control::BusManagementNotify( IOFireWireLibDeviceRef interface, FWClientCommandID commandID )
{
	OSStatus status = noErr;

	Control* me = (Control*)((*interface)->GetRefCon(interface)) ;

	if( (me->mChannelActive) && !(me->mStartStopIsochChannelCommandObjectsInUse) )
	{
		// Command objects now in use.
		me->mStartStopIsochChannelCommandObjectsInUse = true ;

		// Stop channel.
		me->mChannelActive = false ;

		(*me->mIsochChannelRef)->Stop(me->mIsochChannelRef) ;

		// we used to have a completion routine which was called with the channel was stopped,
		// but the channel stop call is synchronous, so we'll just do the work of the
		// completion routine here:
		me->mChannelActive = true ;
		(*me->mIsochChannelRef)->Start(me->mIsochChannelRef) ;

		// do what the completion routine's completion routine used to do:
		me->mStartStopIsochChannelCommandObjectsInUse = false;
	}

	// Complete FireWire client command.
	(*(me->mInterface))->ClientCommandIsComplete(me->mInterface, commandID, status) ;

	return status;
}

/************************************************************************/
/*!
  @function   InitIsochPort
  @abstract   Initializes an isochronous port for the FireWire CCM camera.
  If the request is for the talking port, this routine sets up the camera's
  isochronous port.  If the request is for the listening port, this routine
  sets up the local node's isochronous port.
 */
/************************************************************************/
IOReturn Control::InitIsochPort( IOFireWireLibIsochPortRef interface, IOFWSpeed inSpeed, UInt32 inChannelNum )
{
	OSStatus status = noErr;

	Control* me = (Control*)((*interface)->GetRefCon( interface )) ;
	me->mFWCCMReg = ( inChannelNum << 28) + (inSpeed << 24);

	FWAddress address(0xFFFF, me->mRegBaseAddress + 0x060C);
	(*me->mAsyncWriteCommandObjectRef)->SetTarget( (IOFireWireLibCommandRef) me->mAsyncWriteCommandObjectRef, & address );

	usleep(100000);
	status = (*me->mAsyncWriteCommandObjectRef)->Submit( (IOFireWireLibCommandRef)me->mAsyncWriteCommandObjectRef);

	message( "about to setup the firewire camera" );
	me->setupFWCamera();

	return status;
}

/************************************************************************/
/*!
  @function   IsochPortGetSupported
  @abstract   Setup the channel ID to be sued for transmission
 */
/************************************************************************/
IOReturn Control::IsochPortGetSupported( IOFireWireLibIsochPortRef interface, IOFWSpeed* outMaxSpeed, UInt64* outChanSupported )
{    
	Control* me = (Control*)((*interface)->GetRefCon( interface) );
	me->mSupportedChannelNum = ((UInt64)1) << (63 - me->mChannel);
	*outChanSupported = me->mSupportedChannelNum;
	me->mSpeed = kFWSpeed400MBit;
	*outMaxSpeed = me->mSpeed;

	return kIOReturnSuccess;
}

/************************************************************************/
/*!
  @function   ReleaseIsochPort
  @abstract   This procedure releases resources allocated for the isochronous 
  channel. If the channel is a listener, this routine will release the local
  isochronous port.
 */
/************************************************************************/
IOReturn Control::ReleaseIsochPort(  IOFireWireLibIsochPortRef ) { return kIOReturnSuccess ; }

/************************************************************************/
/*!
  @function   StartIsochPort
  @abstract   This procedure starts the isochronous channel.  If the 
  channel is a talker, this procedure will program the camera to start 
  sending isochronous data.  If the channel is a listener, this procedure 
  will start the local isochronous port.
 */
/************************************************************************/
IOReturn Control::StartIsochPort( IOFireWireLibIsochPortRef  interface )  
{
	IOFireWireLibWriteCommandRef  startAsyncCommandRef;
	OSStatus status = noErr;

	Control* me = (Control*)((*interface)->GetRefCon( interface) );

	me->mFWCCMReg         = 0x80000000;
	startAsyncCommandRef  = me->mStartAsyncCommandObjectRef;

	FWAddress address(0xFFFF, me->mRegBaseAddress + 0x0614);
	(*startAsyncCommandRef)->SetTarget( (IOFireWireLibCommandRef) startAsyncCommandRef, & address);

	usleep(100000);
	status = (*startAsyncCommandRef)->Submit( (IOFireWireLibCommandRef)startAsyncCommandRef );

	return status;
}

/************************************************************************/
/*!
  @function   StopIsochPort
  @abstract   This procedure stops the isochronous channel.  If the 
  channel is a talker, this procedure will program the camera to stop 
  sending isochronous data.  If the channel is a listener, this procedure 
  will stop the local isochronous port.
 */
/************************************************************************/
IOReturn Control::StopIsochPort( IOFireWireLibIsochPortRef  interface )
{
	IOFireWireLibWriteCommandRef  stopAsyncCommandRef ;
	UInt32 *pFWCCMReg;
	OSStatus status = noErr;

	Control* me = (Control*)((*interface)->GetRefCon( interface) );

	pFWCCMReg  = &me->mFWCCMReg;
	*pFWCCMReg = 0x00000000;
	FWAddress address(0xFFFF, me->mRegBaseAddress + 0x0614);

	stopAsyncCommandRef  = me->mStopAsyncCommandObjectRef;

	(*stopAsyncCommandRef)->SetRefCon( (IOFireWireLibCommandRef) stopAsyncCommandRef, me);
	(*stopAsyncCommandRef)->SetTarget( (IOFireWireLibCommandRef) stopAsyncCommandRef, & address);

	usleep(100000);
	status = (*stopAsyncCommandRef)->Submit( (IOFireWireLibCommandRef)stopAsyncCommandRef );

	return status;
}

////////////////////////////////////////////////////////////////////////////////
//
// FWCCMsetupDCLProgram
//
//   This routine is called once each time we are going to start up the DCL
//   program.  This routine sets the callProcs depending on the usage of
//   the program, and it resets all of the jumps within the program.
//
//   See extensive comments in FWCCMWriteDCLProgram for more information.
//
////////////////////////////////////////////////////////////////////////////////
IOReturn Control::setupDCLProgram( DCLCallCommandProcPtr    callProc)
{
	DCLCallProcPtr    pDCLCallProc;
	DCLJumpPtr        pDCLJump;
	IOReturn result = kIOReturnSuccess;  
	DCLLabelPtr       pTargetDCLLabel = nil;

	for( int frameNum = 0; frameNum < kNumFrameBufs; frameNum++ ) 
	{
		// Get DCL pointers we saved earlier:
		if ((frameNum+1) < kNumFrameBufs) pTargetDCLLabel    = mPreFrameDCLLabel[frameNum+1];

		pDCLJump            = mPostFrameDCLJump[frameNum];
		pDCLCallProc        = mPostFrameDCLCallProc[frameNum];
		pDCLCallProc->proc  = callProc ;

		// NOTE: Once the local isoch port has been created, we have to call ModifyJumpDCL() to change jumps. 
		// This makes sure the copy of the DCL program in the kernel is modified properly. Directly manipulating 
		// the user DCL program will have no effect.
		if (result == kIOReturnSuccess)
		{ 
			result = ( *mIsochPortRef )->ModifyJumpDCL( mIsochPortRef, pDCLJump, pTargetDCLLabel );
		}
		if ( result != kIOReturnSuccess )
		{
			fprintf( stderr, "Control.cpp: setupDCLProgram: error 0x%x at ModifyJumpDCL for frame no.%d\n", result, frameNum );
		}
	}

	// Do the first frame first
	mPendingFrameNum       = 0;
	mPendingFrameCount     = 0;
	mPendingFrameFragments = 0;

	return result;
} 

////////////////////////////////////////////////////////////////////////////////
//
// FWCCMsetDCLProgramEndFrame
//
//   This routine rewrites the jumps in the running DCL program to place the
//   specified frame buffer at the end of the program, and make it loop to itself.
//   The previous self-looping end frame is also fixed up.
//
//   See extensive comments in FWCCMWriteDCLProgram for more information.
//

void Control::setDCLProgramEndFrame( UInt32            endFrameNum)
{
	UInt32            prevFrameNum;

	prevFrameNum     = (endFrameNum + (kNumFrameBufs - 1)) % kNumFrameBufs;
	(*mIsochPortRef)->ModifyJumpDCL(mIsochPortRef,
					mPostFrameDCLJump[prevFrameNum],
					mPreFrameDCLLabel[endFrameNum]) ;

}
