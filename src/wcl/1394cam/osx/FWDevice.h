#ifndef __CFOX__DEVICE__FWDEVICE_DOT_H__INCLUDED__
#define __CFOX__DEVICE__FWDEVICE_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
  @header     FWDevice
  @abstract   FWDevice class header
  @discussion Simple encapsulation of a Firewire device data
    
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

namespace cfox
{
  struct FWDevice
  {
    typedef io_service_t                    service_t;
    typedef IOFireWireLibDeviceRef          device_ref_t;
    typedef IOFireWireLibDCLCommandPoolRef  dcl_command_pool_t;
    typedef DCLCommandStruct**              dcl_command_struct_t;

    service_t               service;
    device_ref_t            interface;
    unsigned long           address;
    dcl_command_pool_t      dclCommandPool;
    dcl_command_struct_t    dclCommandStruct;

    FWDevice() : service(0),interface(0),address(0),dclCommandPool(0),dclCommandStruct(0) {}
  };
}

#endif