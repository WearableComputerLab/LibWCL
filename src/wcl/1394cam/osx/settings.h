#ifndef __CFOX__CAMERA__SETTINGS_DOT_H__INCLUDED__
#define __CFOX__CAMERA__SETTINGS_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
  @header     Settings
  @abstract   Settings class header
  @discussion Settings for Firewire Camera
    
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
  // ///////////////////////////////////////////////////////////////////////////
  // Camera Mode & Format binder
  // ///////////////////////////////////////////////////////////////////////////
  struct Mode
  {    
    size_t mode,format;
    Mode(size_t m=0,size_t f=5) : mode(m),format(f) {}
  };
  
  // ///////////////////////////////////////////////////////////////////////////
  // Camera Mode & Format binder : classic value
  // ///////////////////////////////////////////////////////////////////////////
  static Mode Mode_160x120_YUV444   = Mode(0,0);
  static Mode Mode_320x240_YUV422   = Mode(0,1);
  static Mode Mode_640x480_YUV411   = Mode(0,2);
  static Mode Mode_640x480_YUV422   = Mode(0,3);
  static Mode Mode_640x480_RGB      = Mode(0,4);
  static Mode Mode_640x480_Mono8    = Mode(0,5);
  static Mode Mode_640x480_Mono16   = Mode(0,6);
  static Mode Mode_Reserved_07      = Mode(0,7);
  static Mode Mode_800x600_YUV422   = Mode(1,0);
  static Mode Mode_800x600_RGB      = Mode(1,1);
  static Mode Mode_800x600_Mono8    = Mode(1,2);
  static Mode Mode_1024x768_YUV422  = Mode(1,3);
  static Mode Mode_1024x768_RGB     = Mode(1,4);
  static Mode Mode_1024x768_Mono8   = Mode(1,5);
  static Mode Mode_800x600_Mono16   = Mode(1,6);
  static Mode Mode_1024x768_Mono16  = Mode(1,7);
  static Mode Mode_1280X960_YUV422  = Mode(2,0);
  static Mode Mode_1280X960_RGB     = Mode(2,1);
  static Mode Mode_1280X960_Mono8   = Mode(2,2);
  static Mode Mode_1600X1200_YUV422 = Mode(2,3);
  static Mode Mode_1600X1200_RGB    = Mode(2,4);
  static Mode Mode_1600X1200_Mono8  = Mode(2,5);
  static Mode Mode_1280X960_Mono16  = Mode(2,6);
  static Mode Mode_1600X1200_Mono16 = Mode(2,7);
  static Mode Mode_7_0              = Mode(7,0);
  static Mode Mode_7_1              = Mode(7,1);
  static Mode Mode_7_2              = Mode(7,2);
  static Mode Mode_7_3              = Mode(7,3);
  static Mode Mode_7_4              = Mode(7,4);
  static Mode Mode_7_5              = Mode(7,5);
  static Mode Mode_7_6              = Mode(7,6);
  static Mode Mode_7_7              = Mode(7,7);

  // ///////////////////////////////////////////////////////////////////////////
  // Camera FPS binder
  // ///////////////////////////////////////////////////////////////////////////
  struct FPS
  {
    size_t fps;
    FPS(size_t f=4) : fps(f) {}
  };
  
  // ///////////////////////////////////////////////////////////////////////////
  // Camera FPS binder : classic value
  // ///////////////////////////////////////////////////////////////////////////
  static FPS FPS_1_875 = FPS(0);
  static FPS FPS_3_75  = FPS(1);
  static FPS FPS_7_5   = FPS(2);
  static FPS FPS_15    = FPS(3);
  static FPS FPS_30    = FPS(4);
  static FPS FPS_60    = FPS(5);
  
  // ///////////////////////////////////////////////////////////////////////////
  // Camera Specific options binder
  // ///////////////////////////////////////////////////////////////////////////
  struct Spec
  {
    bool broadcaster;
    bool specific;
    Spec(  bool b=true, bool sp=false) : broadcaster(b),specific(sp) {}
  };
    
  // ///////////////////////////////////////////////////////////////////////////
  // Camera Specific options binder : classic value
  // ///////////////////////////////////////////////////////////////////////////
  static Spec BasicCamera                   = Spec(true,true);
  static Spec NonConformantCamera           = Spec(true,false);
  static Spec ListeningCamera               = Spec(false,true);
  static Spec NonConformantListeningCamera  = Spec(false,false);
    
  class Settings
  {
    public:

    Settings( const Mode& m=Mode(), const FPS& f=FPS(), size_t channel = 0, const Spec& sp = Spec() ) 
            : mMode(m.mode), mFormat(m.format), mFPS(f.fps), 
              mChannel(channel), mBroadcaster(sp.broadcaster), mSpecific(sp.specific) {}

    Settings( size_t m,size_t f, size_t fp, size_t ch, bool bc, bool sp)            
            : mMode(m), mFormat(f), mFPS(fp), 
              mChannel(ch), mBroadcaster(bc), mSpecific(sp) {}
              
    Settings( const Settings& s ) { clone(s); }

    Settings& operator=( const Settings& s ) 
    { 
      if(this != &s) { clone(s); } 
      return *this;
    }

    virtual    ~Settings() {}

    size_t getMode()        const { return mMode;           }
    size_t getFormat()      const { return mFormat;         }
    size_t getFPS()         const { return mFPS;            }
    size_t getChannel()     const { return mChannel;        }
    bool   isBroadcaster()  const { return mBroadcaster;    }
    bool   isSpecific()     const { return mSpecific;       }

    void setMode( const size_t& v )       { mMode         = v; }
    void setFormat( const size_t& v )     { mFormat       = v; }
    void setFPS( const size_t& v )        { mFPS          = v; }
    void setChannel( const size_t& v )    { mChannel      = v; }
    void setBroadcaster( const bool& v )  { mBroadcaster  = v; }
    void setSpecific( const bool& v )     { mSpecific     = v; }

    protected : 

    void clone( const Settings& s )
      {
      mMode         = s.mMode;
      mFormat       = s.mFormat;
      mFPS          = s.mFPS;
      mChannel      = s.mChannel;
      mBroadcaster  = s.mBroadcaster;
      mSpecific     = s.mSpecific;
    }

    private:

    size_t mMode;
    size_t mFormat;
    size_t mFPS;
    size_t mChannel;
    bool   mBroadcaster;
    bool   mSpecific;
  };
}

#endif