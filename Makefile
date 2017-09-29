
CC = gcc

LIBS = -lresolv  -lpthread\
	/home/bhavesh/Desktop/CSE533/hw1/libs/unpv13e/libunp.a\
	
FLAGS = -g -O2

CFLAGS = ${FLAGS} -I/home/bhavesh/Desktop/CSE533/hw1/libs/unpv13e/lib

all: client server
	#echoclient timeclient


server:
	${CC} ${CFLAGS} -o server srv.c ${LIBS}

client:
	${CC} ${CFLAGS} -v -o client cli.c ${LIBS}

echoclient:
	${CC} ${CFLAGS} -v -o echocli echocli.c ${LIBS}

timeclient:
	${CC} ${CFLAGS} -v -o timecli timecli.c ${LIBS}

clean:
	rm server client echocli timecli

