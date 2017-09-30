
CC = gcc

LIBS = -lresolv -lpthread\
	/home/bhavesh/Desktop/CSE533/hw1/libs/unpv13e/libunp.a\
	
FLAGS = -g -O2

CFLAGS = ${FLAGS} -I/home/bhavesh/Desktop/CSE533/hw1/libs/unpv13e/lib

all: cli srv echocli timecli


srv:
	rm -rf ./bin/srv
	${CC} ${CFLAGS} -o bin/srv src/srv.c ${LIBS}

cli:
	rm -rf ./bin/cli
	${CC} ${CFLAGS} -v -o bin/cli src/cli.c ${LIBS}

echocli:
	rm -rf ./bin/echocli
	${CC} ${CFLAGS} -v -o bin/echocli src/echocli.c ${LIBS}

timecli:
	rm -rf ./bin/timecli
	${CC} ${CFLAGS} -v -o bin/timecli src/timecli.c ${LIBS}

clean:
	rm -rv bin/*
