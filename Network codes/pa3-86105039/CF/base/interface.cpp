//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */

#include "interface.h"


Interface::Interface () {
	memset (mac, 0, MAC_ADDRESS_LENGTH);
	ip = 0;
	mask = 0;
}

void Interface::init (uint64 mac, uint32 ip, uint32 mask) {
	memcpy (this->mac, &mac, MAC_ADDRESS_LENGTH);
	this->ip = ip;
	this->mask = mask;
}

void Interface::printInterfaceInformation () const {
	PRINT ("-- Type: Ethernet interface");
	char str[3 * 6];
	for (int j = 0; j < 6; ++j) {
		int b1 = (mac[j] >> 4) & 0x0F;
		int b2 = (mac[j]) & 0x0F;
		str[3 * j] = TO_HEX (b1);
		str[3 * j + 1] = TO_HEX (b2);
		str[3 * j + 2] = ':';
	}
	str[3 * 5 + 2] = '\0';
	PRINT ("-- MAC address: " << str);
	PRINT ("-- IP address: " << (ip >> 24) << "." << ((ip >> 16) & 0xFF) << "."
				<< ((ip >> 8) & 0xFF) << "." << (ip & 0xFF));
	PRINT ("-- Network mask: " << (mask >> 24) << "." << ((mask >> 16) & 0xFF) << "."
				<< ((mask >> 8) & 0xFF) << "." << (mask & 0xFF));
}

