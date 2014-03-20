//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */

#include "machine.h"

#include "interface.h"
#include "frame.h"
#include "cf.h"

Machine::Machine (const ClientFramework *cf, int count) {
	this->cf = cf;
	iface = new Interface[countOfInterfaces = count];
}

Machine::~Machine () {
	delete iface;
}

void Machine::initInterface (int index, uint64 mac, uint32 ip, uint32 mask) {
	if (0 <= index && index < countOfInterfaces) {
		iface[index].init (mac, ip, mask);
	}
}

int Machine::getCountOfInterfaces () const {
	return countOfInterfaces;
}

void Machine::printInterfacesInformation () const {
	PRINT ("==========================================");
	for (int i = 0; i < countOfInterfaces; ++i) {
		if (i > 0) {
			PRINT ("---------------------");
		}
		PRINT ("Interface " << i << ":");
		iface[i].printInterfaceInformation ();
	}
	PRINT ("==========================================");
}

void Machine::setCustomInformation (const std::string &info) {
	customInfo = info;
}

const std::string &Machine::getCustomInformation () const {
	return customInfo;
}

bool Machine::sendFrame (Frame frame, int ifaceIndex) const {
	return cf->sendFrame (frame, ifaceIndex);
}

