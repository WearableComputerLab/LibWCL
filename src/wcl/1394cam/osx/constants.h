#ifndef __CFOX__MISC__CONSTANTS_DOT_H__INCLUDED__
#define __CFOX__MISC__CONSTANTS_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
  @header     Constants
  @abstract   General constants values for C+FOX project
  @discussion This file contains allt he enumeration used throughout
  the whole C+FOX source code.
  
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

namespace cfox
{
  //////////////////////////////////////////////////////////////////////////////
  /*
    Aquisition mode
  */
  //////////////////////////////////////////////////////////////////////////////
  enum {  MODE_TIME        = 0x01,
          MODE_FRAME       = 0x02,
          MODE_TRIGGER     = 0x04,
          MODE_RELATIVE    = 0x08,
          MODE_ABSOLUTE    = 0x10,
          MODE_RESET_TIME  = 0x20,
          MODE_RESET_FRAME = 0x40
        };

  //////////////////////////////////////////////////////////////////////////////
  /*
    Internal Error code
  */
  //////////////////////////////////////////////////////////////////////////////           
  enum {  OK           = 0x00,
          ERR          = 0x01,
          QUEUE_FULL   = 0x02,
          QUEUE_MAX    = 0x0A,
          TRIGGER_OFF  = 0x00,
          TRIGGER_WAIT = 0x01,
          TRIGGER_ON   = 0x02 
        };

  //////////////////////////////////////////////////////////////////////////////
  /*
    Internal Firewire Constants
  */
  //////////////////////////////////////////////////////////////////////////////
  enum {  kFWCCMSpecID     = 0x00a02d,
          kFWCCMSwVersIDv1 = 0x000100,
          kFWCCMSwVersIDv2 = 0x000101,
          kFWCCMSwVersIDv3 = 0x000102    
        };
}

#endif