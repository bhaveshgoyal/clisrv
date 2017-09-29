
CC = gcc

LIBS = -lresolv -lpthread\
	/home/bhavesh/Desktop/CSE533/hw1/libs/unpv13e/libunp.a\
	
FLAGS = -g -O2

CFLAGS = ${FLAGS} -I/home/bhavesh/Desktop/CSE533/hw1/libs/unpv13e/lib

all: client server echoclient
	#timeclient


server:
	rm -rf server
	${CC} ${CFLAGS} -o bin/server server.c ${LIBS}

client:
	rm -rf client
	${CC} ${CFLAGS} -v -o bin/client client.c ${LIBS}

echoclient:
	rm -rf echocli
	${CC} ${CFLAGS} -v -o bin/echocli echocli.c ${LIBS}

timeclient:
	rm -rf timecli
	${CC} ${CFLAGS} -v -o bin/timecli timecli.c ${LIBS}

clean:
	rm -rv bin/*
	#timecli

