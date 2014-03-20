//			In the name of GOD
/**
 * Copyright (C) 2009-2010 Behnam Momeni.
 * All rights reserved.
 *
 * This file is part of the HVNS project.
 */

#ifndef HVNS_DEF_H_
#define HVNS_DEF_H_

#include <iostream>

#include <string.h>
#include <stdint.h>

#define PRINT(X) std::cout << X << std::endl

#define TO_HEX(DEC_NUM) (((DEC_NUM) < 10) ? ((DEC_NUM) + '0') : ((DEC_NUM) - 10 + 'A'))

#define forever for (;;)

typedef uint8_t byte;

typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int32_t int32;

#endif /* hvnsdef.h */

