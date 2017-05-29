CC=/usr/bin/gcc
AR=/usr/bin/ar
CP=/bin/cp
MKDIR=/bin/mkdir
RM=/bin/rm

CFLAGS=-g3 -Wall -lm

BUILD_DIR=../build

all: init
	${CC} ${CFLAGS} -I. -c -o ${BUILD_DIR}/xbee_uart.o xbee_uart.c
	${CC} ${CFLAGS} -I. -c -o ${BUILD_DIR}/xbee_lib.o xbee_lib.c
	${AR} -rsc ${BUILD_DIR}/libxbee.a ${BUILD_DIR}/xbee_uart.o ${BUILD_DIR}/xbee_lib.o
	${CP} *.h ${BUILD_DIR}

init:
	${MKDIR} -p ${BUILD_DIR}

clean:
	${RM} ${BUILD_DIR}/*
