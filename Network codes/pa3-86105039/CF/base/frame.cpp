//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */

#include "frame.h"

Frame::Frame (uint32 _length, byte *_data) :
	length (_length), data (_data) {
}

Frame::~Frame () {
	// ownership of the data is not with frame; so do not delete it...
}

