//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */


#ifndef _S_M_H_
#define _S_M_H_

#include "machine.h"

class SimulatedMachine : public Machine {
public:
	SimulatedMachine (const ClientFramework *cf, int count);

	virtual void initialize ();
	virtual void run ();
	virtual void processFrame (Frame frame, int ifaceIndex);
};

#endif /* sm.h */

