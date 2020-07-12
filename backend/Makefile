CC=gcc

CFLAGS= -g

all: backend 

backend: server.c routing.c
	${CC} ${CFLAGS} -o backend server.c routing.c

clean:
	rm -rf *.dSYM backend