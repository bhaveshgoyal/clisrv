
CC = gcc

LIBS = -lresolv -lnsl -lsocket -lpthread\
		/home/courses/cse533/Stevens/unpv13e_solaris2.10/libunp.a\

FLAGS = -g -O2

CFLAGS = ${FLAGS} -I/home/courses/cse533/Stevens/unpv13e_solaris2.10/lib

all: cli srv echocli timecli


srv:
	rm -rf ./bin/srv
	${CC} ${CFLAGS} -o bin/srv src/srv.c ${LIBS}

cli:
	rm -rf ./bin/cli
	${CC} ${CFLAGS}  -o bin/cli src/cli.c ${LIBS}

echocli:
	rm -rf ./bin/echocli
	${CC} ${CFLAGS}  -o bin/echocli src/echocli.c ${LIBS}

timecli:
	rm -rf ./bin/timecli
	${CC} ${CFLAGS}  -o bin/timecli src/timecli.c ${LIBS}

clean:
	rm -rv bin/*
