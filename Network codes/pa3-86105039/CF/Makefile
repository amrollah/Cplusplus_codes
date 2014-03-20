#################################################
##              In the name of GOD
##
## Copyright (C) 2009 Behnam Momeni.
## All rights reserved.
##
## This file is part of the HVNS project.
##
#################################################

COMPILER = g++
FLAGS = -Wall -g -Ibase -Iuser
LIBS = -lpthread

NODE_TARGET = node.out
AP_TARGET = ap.out
TARGETS = ${NODE_TARGET} ${AP_TARGET}

BASE_SOURCES = base/main.cpp \
	base/cf.cpp \
	base/interface.cpp \
	base/machine.cpp \
	base/frame.cpp

NODE_SOURCES = user/node.cpp

AP_SOURCES = user/ap.cpp

BASE_OBJ = ${BASE_SOURCES:.cpp=.o}
BASE_OBJECTS = ${BASE_OBJ:.c=.o}

NODE_OBJ = ${NODE_SOURCES:.cpp=.o}
NODE_OBJECTS = ${NODE_OBJ:.c=.o} ${BASE_OBJECTS}

AP_OBJ = ${AP_SOURCES:.cpp=.o}
AP_OBJECTS = ${AP_OBJ:.c=.o} ${BASE_OBJECTS}

OBJECTS = ${BASE_OBJECTS} ${NODE_OBJECTS} ${AP_OBJECTS}

all: ${TARGETS}

${NODE_TARGET}: ${NODE_OBJECTS}
	${COMPILER} ${FLAGS} -o ${NODE_TARGET} ${NODE_OBJECTS} ${LIBS}

${AP_TARGET}: ${AP_OBJECTS}
	${COMPILER} ${FLAGS} -o ${AP_TARGET} ${AP_OBJECTS} ${LIBS}

clean:
	rm -f ${OBJECTS} ${TARGETS} *~ base/*~ user/*~

.cpp.o:
	${COMPILER} ${FLAGS} -c $< -o $@
.c.o:
	${COMPILER} ${FLAGS} -c $< -o $@

