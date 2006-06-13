#ifndef __CFOX__CAMERA__CAMERA_DOT_H__INCLUDED__
#define __CFOX__CAMERA__CAMERA_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
  @header     Camera
  @abstract   Camera class header
  @discussion Main class for Firewire Camera access
  
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
#include "settings.h"
#include "formats.h"
#include "constants.h"
#include "../debug.h"

namespace cfox
{
  class Control;

  //////////////////////////////////////////////////////////////////////////////
  /*!
    @class Camera
    @abstract    Firewire Camera Interface
    @discussion  Camera class is designed to provide a flexible way to set up 
    Firewire Digital Camera and get frame from them. Various mode of acquisition
    are available and full range of FW Camera options are included.
  */
  //////////////////////////////////////////////////////////////////////////////
  class Camera
  {
    public :

    typedef unsigned char Pixel;
    typedef Pixel*        Frame;
    
    Camera();
    Camera( const Mode& m, const FPS& f, size_t channel, const Spec& sp);
    Camera( const Settings& opt );

    virtual ~Camera();

    IOReturn  start();
    IOReturn  stop();
    Frame     getFrame( int mode, int frame_no, long time );
    bool      copyFrame( Frame fr, int mode = MODE_FRAME, int frame_no = 0, long time = 0);
    
    size_t    getSize()   const;
    size_t    getWidth()  const;
    size_t    getHeight() const;

    Camera& operator>>( Frame& dst );
    Camera& operator>>( Settings& dst )         { dst = options; return *this; }
    Camera& operator<<( const Settings& src )   { options = src; return *this; }

   protected :

    Control				*camControl;

    private :

    unsigned char*  curFrame;
    float           framesPerSecond;
    int             curBuffer;
    bool            firstFrameOK;
    Settings        options;
  };
}

#endif
