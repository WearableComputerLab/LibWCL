#ifndef __CFOX__DEVICE__FWCONTROLLER_DOT_H__INCLUDED__
#define __CFOX__DEVICE__FWCONTROLLER_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
  @header     FWController
  @abstract   FWController class header
  @discussion Main class for Firewire control
    
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

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/firewire/IOFireWireLibIsoch.h>
#include <mach/mach.h>
#include <vector>

#include "FWDevice.h"
#include "singleton.h"
#include "../Debug.h"

namespace cfox
{
  //////////////////////////////////////////////////////////////////////////////
  /*!
    @class FWController
    @abstract    Firewire Unit Inspector
    @discussion  FWController class is designed to inspects
    Firewire channel and found video device. From there it
    generates a list of potential usable camera and exposes
    an interface that allows FrameGrabber class to select one
    of this device, initialize it and use it as frame sources.

    Based on that, FWController acts like a Gateway pattern
    between the client code using FrameGrabber and the IOKit
    API published by Apple.

    This code is an adaptation of the FWCamAckiz code from 
    Adrian Daerr.
  */
  //////////////////////////////////////////////////////////////////////////////
  class FWController : public Singleton<FWController>
  {
    friend class Singleton<FWController>;

    public:

    typedef std::vector<FWDevice>   device_list;

    static const size_t kNumFrameBufs           = 5;
    static const size_t kLogMaxPacketsPerFrame  = 12;
    static const size_t kMaxPacketsPerFrame     = ( 1 << kLogMaxPacketsPerFrame ) - 1;
    static const size_t kMaxTransferDCLs        = kMaxPacketsPerFrame;
    static const size_t kDCLProgramSize         = (sizeof(DCLTransferPacket) + sizeof(DCLJump) + sizeof(DCLLabel))*(kMaxTransferDCLs + 40);

    virtual ~FWController();

    FWDevice::service_t    getService( size_t idx )  const;
    size_t                 getServicesAmount()       const;

    FWDevice::device_ref_t acquireService( size_t idx, IOFireWireLibDCLCommandPoolRef& dcl, DCLCommandStruct**& udcl  );
    void                   releaseService( size_t idx );

    void                   getUnitInfo( size_t idx, UInt32& adress );

    protected:

    device_list     mDevices;
    IOCFPlugInInterface**   mCFPlugInInterface;

    FWController();

    private:
    void findUnit( mach_port_t masterPort );
    int getCameras( CFMutableDictionaryRef dict, mach_port_t masterPort );
    void prepareDCL( size_t idx,  IOFireWireLibDCLCommandPoolRef& dcl, DCLCommandStruct**& udcl );
  };
}

#endif
