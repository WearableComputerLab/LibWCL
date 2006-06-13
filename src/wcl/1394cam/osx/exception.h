#ifndef __CFOX__MISC__EXCEPTION_DOT_H__INCLUDED__
#define __CFOX__MISC__EXCEPTION_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
  @header FWException
  @abstract   FWException class header
  
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

#include <exception>
#include <string>

#define DECLARE_EXCEPTION( NAME, TEXT ) \
class NAME : public std::exception \
{\
    public:\
    NAME() {}\
    virtual ~NAME() throw() {}\
    virtual const char* what() const throw() { return TEXT; }\
}

DECLARE_EXCEPTION(NoServicesFound, "No Camera Services were found in Dictionnary.");
DECLARE_EXCEPTION(InvalidServiceDictionary, "No Services Dictionnary were found on this device.");
DECLARE_EXCEPTION(InvalidIOMasterPort, "IO Master Port is invalid.");
DECLARE_EXCEPTION(InvalidServiceRequested,"Invalid service requested.");
DECLARE_EXCEPTION(IsochChannelFailed, "Isochronous Channel creation failed.");
DECLARE_EXCEPTION(QueryInterfaceFailed, "Interface query failed.");
DECLARE_EXCEPTION(InterfacePlugInCreationFailed, "Unable to create plugin for given interface.");
DECLARE_EXCEPTION(NotEnoughMemory, "Not enough memory for operation.");
DECLARE_EXCEPTION(ModeSevenFailure, "Mode 7 failed to be initialized.");
DECLARE_EXCEPTION(InvalidROI,"Invalid ROI position and/or size.");
DECLARE_EXCEPTION(InvalidPacketsPerFrame,"Packet per frames exceed maximum allowed.");
DECLARE_EXCEPTION(FrameBufferAllocationFailure,"Unable to allocate memory for frame buffers.");

#endif