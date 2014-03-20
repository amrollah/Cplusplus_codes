//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */


#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "hvnsdef.h"

class Interface {
public:
	static const int MAC_ADDRESS_LENGTH = 6;

	byte mac[MAC_ADDRESS_LENGTH];
	uint32 ip;
	uint32 mask;

	Interface ();

	void init (uint64 mac, uint32 ip, uint32 mask);

	void printInterfaceInformation () const;
};

#endif /* interface.h */

