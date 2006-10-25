/**
 * The Manufacturer class represents contains 
 * a list of MAC->Manufacturer names tuples.
 * The class provides specific details to deal with tuples
 *
 * $ClearChain: projects/clearchain/src/network/Manufacturer.h,v 1.3 2005/11/30 04:57:49 benjsc Exp $
 *
 * Copyright (C) 2005 Benjamin Close <Benjamin.Close@clearchain.com>
 */

#ifndef WCL_MANUFACTURER_H
#define WCL_MANUFACTURER_H

#include <wcl/network/Network.h>

class Manufacturer
{
public:

	static const char *getManufacturer( const EthernetAddress & );

private:
	Manufacturer();

	char ****machash;

	static Manufacturer *instance;
	static char *manufacturerlist[];

};

#endif
