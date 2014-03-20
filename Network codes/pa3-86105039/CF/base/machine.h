//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */


#ifndef MACHINE_H_
#define MACHINE_H_

#include "hvnsdef.h"

class Interface;
class ClientFramework;
class Frame;

/* Abstract */ class Machine {
private:
	const ClientFramework *cf;
	std::string customInfo;

protected:
	int countOfInterfaces;
	Interface *iface;

public:
	Machine (const ClientFramework *cf, int count);
	virtual ~Machine ();

	virtual void initialize () = 0;
	virtual void run () = 0;

	void initInterface (int index, uint64 mac, uint32 ip, uint32 mask);
	void printInterfacesInformation () const;

	void setCustomInformation (const std::string &info);
	const std::string &getCustomInformation () const;

	int getCountOfInterfaces () const;

	bool /* Synchronized */ sendFrame (Frame frame, int ifaceIndex) const;
	virtual void processFrame (Frame frame, int ifaceIndex) = 0;
};

#endif /* machine.h */

