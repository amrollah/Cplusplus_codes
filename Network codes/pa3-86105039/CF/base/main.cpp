//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */

#include "cf.h"

bool runCF (ClientFramework &cf);

int main (int argc, char *argv[]) {
	ClientFramework::init (argc, argv);

	bool res = runCF (*ClientFramework::getInstance ());

	ClientFramework::destroy ();

	if (res) {
		return 0;
	} else {
		return -1;
	}
}

bool runCF (ClientFramework &cf) {
	cf.printArguments ();

	return cf.connectToServer ()
		&& cf.doInitialNegotiations ()
		&& cf.startSimulation ();
}

