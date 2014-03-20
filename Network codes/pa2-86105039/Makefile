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
LIBS = -lpthread -L. -lcf

TARGET = cf.out

USER_SOURCES = user/sm.cpp

SOURCES = ${USER_SOURCES}
OBJ = ${SOURCES:.cpp=.o}
OBJECTS = ${OBJ:.c=.o}


all: ${OBJECTS}
	${COMPILER} ${FLAGS} -o ${TARGET} ${OBJECTS} ${LIBS}

clean:
	rm -f ${OBJECTS} ${TARGET} *~ user/*~

.cpp.o:
	${COMPILER} ${FLAGS} -c $< -o $@
.c.o:
	${COMPILER} ${FLAGS} -c $< -o $@

