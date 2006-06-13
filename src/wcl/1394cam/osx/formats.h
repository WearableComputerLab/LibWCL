#ifndef __CFOX__MISC__FORMATS_DOT_H__INCLUDED__
#define __CFOX__MISC__FORMATS_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
  @header     Formats
  @abstract   General Formats values for C+FOX project
  @discussion This file contains the table needed to match Firewire
  mode/format code to humanly readable values.
  
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

#include <IOKit/IOKitLib.h>
#include <IOKit/firewire/IOFireWireLibIsoch.h>
#include <IOKit/firewire/IOFireWireLib.h>
#include <Carbon/Carbon.h>
#include <mach/mach.h>
#include <iostream>

namespace cfox
{
  //////////////////////////////////////////////////////////////////////////////
  /*
    Video Mode description. Access is done via :
    VideoMode[mode][format]

    For example : mode 0 & format 5 is 640 X 480 Y Mono 8 bits/pixel
  */
  //////////////////////////////////////////////////////////////////////////////
  static const char 
  VideoMode[8][8][40] = {
    {	
    "160 X 120 YUV(4:4:4) 24 bits/pixel",
    "320 X 240 YUV(4:2:2) 16 bits/pixel",
    "640 X 480 YUV(4:1:1) 12 bits/pixel",
    "640 X 480 YUV(4:2:2) 16 bits/pixel",
    "640 X 480 RGB 24 bits/pixel",
    "640 X 480 Y Mono 8 bits/pixel",
    "640 X 480 Y Mono 16 bits/pixel",
    "Reserved 07"	
    },
    {
    "800 X 600 YUV(4:2:2) 16 bits/pixel",
    "800 X 600 RGB  24 bits/pixel",
    "800 X 600 Y Mono 8 bits/pixel",
    "1024 X 768 YUV(4:2:2) 16 bits/pixel",
    "1024 X 768 RGB 24 bits/pixel",
    "1024 X 768 Y Mono 8 bits/pixel",
    "800 X 600 Y Mono 16 bits/pixel",
    "1024 X 768 Y Mono 16 bits/pixel"	
    },
    {
    "1280 X 960 YUV(4:2:2) 16 bits/pixel",
    "1280 X 960 RGB 24 bits/pixel",
    "1280 X 960 Y Mono 8 bits/pixel",
    "1600 X 1200 YUV(4:2:2) 16 bits/pixel",
    "1600 X 1200 RGB 24 bits/pixel",
    "1600 X 1200 Y Mono 8 bits/pixel",
    "1280 X 960 Mono 16 bits/pixel",
    "1600 X 1200 Mono 16 bits/pixel"	
    },
    {
    "Reserved 30",
    "Reserved 31",
    "Reserved 32",
    "Reserved 33",
    "Reserved 34",
    "Reserved 35",
    "Reserved 36",
    "Reserved 37"
    },
    {	
    "Reserved 40",
    "Reserved 41",
    "Reserved 42",
    "Reserved 43",
    "Reserved 44",
    "Reserved 45",
    "Reserved 46",
    "Reserved 47"
    },
    {
    "Reserved 50",
    "Reserved 51",
    "Reserved 52",
    "Reserved 53",
    "Reserved 54",
    "Reserved 55",
    "Reserved 56",
    "Reserved 57"
    },
    {
    "EXIF Mode"
    "Reserved 61",
    "Reserved 62",
    "Reserved 63",
    "Reserved 64",
    "Reserved 65",
    "Reserved 66",
    "Reserved 67",
    },
    {
    "Format_7 Mode_0",
    "Format_7 Mode_1",
    "Format_7 Mode_2",
    "Format_7 Mode_3",
    "Format_7 Mode_4",
    "Format_7 Mode_5",
    "Format_7 Mode_6",
    "Format_7 Mode_7"	
    }
  };

  //////////////////////////////////////////////////////////////////////////////
  /*
    Video Mode pixel size in byte. Access is done via :
    VideoModeSize[mode][format]

    For example : mode 0 & format 5 is 1 nyte per pixel
  */
  //////////////////////////////////////////////////////////////////////////////
  static const int VideoModeSize[8][8]= {
    {	3,2,3,2,3,1,2,1	},
    { 2,3,1,2,3,1,2,2	},
    { 2,3,1,2,3,1,2,2 },
    { 1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1 },    
    { 1,1,1,1,1,1,1,1 },    
    { 1,1,1,1,1,1,1,1 },    
    { 1,1,1,1,1,1,1,1 }
  };
  
  //////////////////////////////////////////////////////////////////////////////
  /*
    Video Framerate description. Access is done via :
    Fps[framerate]

    For example : framerate 4 is 30fps
    
    The FrameRate enum is used by Settings to determine correct framerate
  */
  //////////////////////////////////////////////////////////////////////////////
  static const char Fps[6][10] = {"1.875 fps","3.75 fps","7.5 fps","15 fps","30 fps","60 fps"};

  //////////////////////////////////////////////////////////////////////////////
  /*
    Video Packet size in byte. Access is done via :
    PacketByteSize[mode][format][framerate]

    For example :  mode 0, format 5 & framerate 4 is 1280 bytes per packet.
  */
  //////////////////////////////////////////////////////////////////////////////
  static const IOByteCount 
  PacketByteSize[3][8][6] = {
    {
    { 0,0,60,120,240,0 },
    { 0,80,160,320,640,0 },
    { 0,240,480,960,1920,0 },
    { 0,320,640,1280,2560,0 },
    { 0,480,960,1920,3840,0 },
    { 0,160,320,640,1280,2560 },
    { 0,320,640,1280,2560,0 },
    { 0,0,0,0,0,0 }
    },
    {
    { 0,500,1000,2000,4000,0 },
    { 0,0,1500,3000,0,0 },
    { 0,0,500,1000,2000,4000 },
    { 384,768,1536,3072,0,0 },
    { 576,1152,2304,0,0,0 },
    { 192,384,768,1536,3072,0 },
    { 0,500,1000,2000,4000,0 },
    { 384,768,1536,3072,0,0 }
    },
    {
    { 640,1280,2560,0,0,0 },
    { 960,1920,3840,0,0,0 },
    { 320,640,1280,2560,0,0 },
    { 1000,2000,4000,0,0,0 },
    { 1500,3000,0,0,0,0 },
    { 500,1000,2000,4000,0,0 },
    { 640,1280,2560,0,0,0 },
    { 1000,2000,4000,0,0,0 } 
    } 
  };

  //////////////////////////////////////////////////////////////////////////////
  /*
    Video Width in pixels. Access is done via :
    Width[mode][format]

    For example :  mode 0, format 5 is 640 pixels wide.
  */
  //////////////////////////////////////////////////////////////////////////////
  static const int  Width[3][8] = {
  { 160,320,640,640,640,640,640,640 },
  { 800,800,800,1024,1024,1024,800,1024 },
  { 1280,1280,1280,1600,1600,1600,1280,1600 } } ;

  //////////////////////////////////////////////////////////////////////////////
  /*
    Video Height in pixels. Access is done via :
    Height[mode][format]

    For example :  mode 0, format 5  is 480 pixels high.
  */
  //////////////////////////////////////////////////////////////////////////////
  static const int Height[3][8] = {	
  { 120,240,480,480,480,480,480,480 },
  { 600,600,600,768,768,768,600,768 },
  { 960,960,960,1200,1200,1200,960,1200 } } ;
}

// --- End of CFox namespace

#endif

// -- End of formats.h