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

#include <iostream>
#include "constants.h"
#include "exception.h"
#include "FWController.h"

using namespace cfox;

/************************************************************************/
/*!
    @method    FWController 
    @abstract   FWController base constructor
    @discussion FWController constructor uses
    a Device Dictionary to find out video device
    on the Firewire bus. From there, it builds an
    internal list of services attached to those
    devices.
*/
/************************************************************************/
FWController::FWController()
{
	// all user-level access of a device begins with a Mach port to
	// communicate with the I/O Kit.
	mach_port_t  masterPort;

	
	// attempt to get a Mach port and then check to see if there were
	// any problems getting it.
	if( kIOReturnSuccess == IOMasterPort( MACH_PORT_NULL, &masterPort ) )
	{
		findUnit(masterPort);
	}
	else
	{
		throw InvalidIOMasterPort();
	}
}

/************************************************************************/
/*!
    @method   ~FWController()  
    @abstract   FWController base destructor
    @discussion Release all memory used by
    the Firewire object and channel. After
    being destroyed, FWController must have
    set all firewire device into a clean
    state for other applications to use them. 
*/
/************************************************************************/
FWController::~FWController() 
{
    if( mCFPlugInInterface ) IODestroyPlugInInterface(mCFPlugInInterface);
}

/************************************************************************/
/*!
    @method     acquireService
    @abstract   FWController main access onto services
    @discussion acquireService allows client to
    control a specific Firewire device found on the
    bus by referencing them by their service ID.
    acquireService performs all the needed initialisation
    of the base device service, letting the client the choice
    on how to hndle IRM and other device options. 
*/
/************************************************************************/
FWDevice::device_ref_t FWController::acquireService( size_t idx, IOFireWireLibDCLCommandPoolRef& dcl, DCLCommandStruct**& udcl )
{
	message( "check that there are actually firewire devices attached to the computer" );
	if( mDevices.size() == 0 )
	{
		gen_fatal( "there are no firewire devices plugged into the computer" );
	}

	SInt32                  score;
	IOFireWireLibDeviceRef  resultInterface = 0;
	ComponentResult         result = noErr;
	
	// get the IOCFPlugInInterface
	result = IOCreatePlugInInterfaceForService( mDevices[idx].service, kIOFireWireLibTypeID, 
			kIOCFPlugInInterfaceID, &mCFPlugInInterface, &score);

	message( "check if the plugin was created sucessfully" );
	if( result == kIOReturnSuccess )
	{
		// get the specific device interface that we want
		HRESULT err = (*mCFPlugInInterface)->QueryInterface(mCFPlugInInterface,
				CFUUIDGetUUIDBytes(kIOFireWireDeviceInterfaceID_v2),(void**) &resultInterface);

		// check if the interface was queried okay.
		if( err == S_OK )
		{
			mDevices[idx].interface = resultInterface;
			prepareDCL(idx,dcl,udcl);

			// connect the device for exclusive access
			IOReturn returnStatus = (*resultInterface)->Open(resultInterface);

			// check to make sure that the device was connected to
			if( returnStatus != noErr )
			{
				gen_fatal( "Failed to connect to firewire device" );
			}
		}
		else
		{
			throw QueryInterfaceFailed();
		}
	}
	else
	{
		throw InterfacePlugInCreationFailed();
	}

	message( "add callback dispatcher to run loop" );
	(*resultInterface)->AddCallbackDispatcherToRunLoop(resultInterface, CFRunLoopGetCurrent() );

	message( "add iso callback to dispatcher to run loop" );
	(*resultInterface)->AddIsochCallbackDispatcherToRunLoop(resultInterface,CFRunLoopGetCurrent());

	return resultInterface;
}

/************************************************************************/
/*!
  @method     releaseService
  @abstract   FWController method to clean up open services. MUST be called
  for each services used trough acquireServices.
 */
/************************************************************************/
void FWController::releaseService( size_t idx )
{
	if( idx < mDevices.size() )
	{
		if( mDevices[idx].dclCommandStruct) delete[] mDevices[idx].dclCommandStruct;
		if( mDevices[idx].dclCommandPool)   (*(mDevices[idx].dclCommandPool))->Release(mDevices[idx].dclCommandPool) ;
		if( mDevices[idx].interface ) 
		{
			(*(mDevices[idx].interface))->Close(mDevices[idx].interface) ;
			(*(mDevices[idx].interface))->Release(mDevices[idx].interface) ;
		}
	}
	else
	{
		throw InvalidServiceRequested();
	}    
}

/************************************************************************/
/*!
  @method     getUnitInfo
  @abstract   Firewire Unit Information retriever
  @discussion Retrieves the Unit Directory and Unit Directory ID
  from a Firewire device on the bus. This identification is needed
  to retrieve the device base address.
 */
/************************************************************************/
void FWController::getUnitInfo( size_t idx, UInt32& adress )
{
	IOFireWireLibConfigDirectoryRef ud   = 0; 
	IOFireWireLibConfigDirectoryRef udid = 0;
	FWAddress                       baseAddress;
	CFStringRef                     text;

	// create a config directory object and return an interface to it.
	ud = ( *mDevices[idx].interface )->GetConfigDirectory( mDevices[idx].interface, CFUUIDGetUUIDBytes( kIOFireWireConfigDirectoryInterfaceID ) );

	// check if the interface was returned.
	if( !ud )
	{
		throw NotEnoughMemory();
	}
	else
	{
		
		( *ud )->GetKeyValue_ConfigDirectory( ud, kConfigUnitDependentInfoKey, &udid, CFUUIDGetUUIDBytes( kIOFireWireConfigDirectoryInterfaceID ), nil );
		(*udid)->GetKeyOffset_FWAddress(udid, 0x00, &baseAddress, &text);

		CFDataRef dataRef;
		CFStringRef stringRef;
		IOReturn returnVal = ( *udid )->GetKeyValue_Data( udid, kConfigTextualDescriptorKey, &dataRef, &stringRef );

		if( returnVal != noErr )
		{
			gen_fatal( "couldn't get the textual descriptor: %d", returnVal );
		}
		else
		{
			message( "textual descriptor: " );
		}

		mDevices[idx].address = baseAddress.addressLo;
		adress = mDevices[idx].address;
	}

	if( udid )  (*udid)->Release(udid);
	if( ud   )  (*ud)->Release(ud);
}

/************************************************************************/
/*!
  @method     findUnit
  @abstract   Search for valid Camera device on the bus
  @discussion Identify camera device on the Firewire bus.
  This identification allows FWController to sort annymous 
  firewire device between camera and non-camera devices. 
 */
/************************************************************************/
void FWController::findUnit( mach_port_t masterPort )
{
	// ----------------------------------------------------------
	// Build Matching Dictionary
	// ----------------------------------------------------------

	// create a dictionary that defines the type of units that we want
	// to grab off the bus
	CFMutableDictionaryRef  dict = IOServiceMatching( "IOFireWireUnit" );
	UInt32                  value ;
	CFNumberRef             cfValue ;

//	value    = kFWCCMSpecID;
//
//	cfValue  = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value) ;

	// narrow down the search criteria and only grab units that have the 
	// specified unit spec id.
//	CFDictionaryAddValue(dict, CFSTR("Unit_Spec_ID"), cfValue) ;
//	CFRelease(cfValue) ;

	// check that the search criteria is all good.
	if( dict )
	{
		// ----------------------------------------------------------
		// Iterates through services and add them to master list
		// ----------------------------------------------------------    
		io_service_t    service;
		FWDevice        device;
		io_iterator_t   iterator;

		// search the I/O registry for any units that match out criteria.
		if( kIOReturnSuccess == IOServiceGetMatchingServices( masterPort, dict, &iterator ) )
		{
			message( "extracting services out of the iterator" );
			service = IOIteratorNext( iterator );

			while( service != NULL )
			{
				device.service = service;
				mDevices.push_back(device);
				service = IOIteratorNext( iterator );
			}
	
			// check if there were any devices connected to the machine
			message( "checking to make sure there are firewire devices connected to the machine" );

			if( mDevices.size() == 0 )
			{
				gen_fatal( "There are no firewire devices connected this machine" );
			}
			else
			{
				message( "Found %d firewire devices", ( int )mDevices.size() );
			}
		}
		else
		{
			gen_fatal( "No services found" );
		}
	}
	else
	{
		throw InvalidServiceDictionary();
	}
}

/************************************************************************/
/*!
  @method     prepareDCL
  @abstract   DCL Command and Structure initializer
  @discussion Allocate and create the DCL Command Pool and
  Structure for the DCL programm of the camera.
 */
/************************************************************************/
void FWController::prepareDCL( size_t idx,  IOFireWireLibDCLCommandPoolRef& dcl, DCLCommandStruct**& udcl )
{
	IOFireWireLibDeviceRef  i = mDevices[idx].interface;

	mDevices[idx].dclCommandPool = (*i)->CreateDCLCommandPool( i, kDCLProgramSize, CFUUIDGetUUIDBytes(kIOFireWireDCLCommandPoolInterfaceID));
	dcl = mDevices[idx].dclCommandPool;

	if( mDevices[idx].dclCommandPool )
	{
		mDevices[idx].dclCommandStruct = new DCLCommandStruct*[kMaxTransferDCLs] ;
		udcl = mDevices[idx].dclCommandStruct;
	}
	else
	{
		throw NotEnoughMemory();
	}
}

/************************************************************************/
/*!
  @method     getService
  @abstract   Service access
  @discussion Send a pointer to a given Firewire IO Service from the bus.
 */
/************************************************************************/
FWDevice::service_t FWController::getService( size_t idx )  const   
{ 
	if(idx < mDevices.size() )
	{
		return mDevices[idx].service;
	}
	else
	{
		throw InvalidServiceRequested();
	}
}

/************************************************************************/
/*!
  @method     getServicesAmount
  @abstract   Service enumeration
  @discussion Retrieve the total number of available DC device found.
 */
/************************************************************************/
size_t              FWController::getServicesAmount() const   
{ 
	return mDevices.size();       
}
