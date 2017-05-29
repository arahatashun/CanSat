CC=/usr/bin/gcc
AR=/usr/bin/ar
CP=/bin/cp
MKDIR=/bin/mkdir
RM=/bin/rm

CFLAGS=-g3 -Wall -lm

BUILD_DIR=../build

all: init
	${CC} ${CFLAGS} -I. -c -o ${BUILD_DIR}/xbeeuart.o xbeeuart.c
	${CC} ${CFLAGS} -I. -c -o ${BUILD_DIR}/xbeelib.o xbeelib.c
	${AR} -rsc ${BUILD_DIR}/libxbee.a ${BUILD_DIR}/xbeeuart.o ${BUILD_DIR}/xbeelib.o
	${CP} *.h ${BUILD_DIR}
	@echo
	@echo
	@echo RUN TESTS SHOULD BE A GOOD IDEA...
	@echo
	@echo make tests
	@echo
	@echo

init:
	${MKDIR} -p ${BUILD_DIR}

install:
	ln -s `pwd`/${BUILD_DIR}/libxbee.a /usr/lib/libxbee.a
	ln -s `pwd`/${BUILD_DIR}/xbeeuart.h /usr/include/xbeeuart.h

clean:
	${RM} ${BUILD_DIR}/*
