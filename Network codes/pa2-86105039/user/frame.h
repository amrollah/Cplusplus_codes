//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */


#ifndef FRAME_H_
#define FRAME_H_

#include "hvnsdef.h"

class Frame {
public:
	uint32 length;
	byte *data;

	Frame (uint32 _length, byte *_data);
	virtual ~Frame ();
};

#endif /* frame.h */

