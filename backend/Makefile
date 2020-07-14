CC=gcc

CFLAGS= -g

all: backend 

backend: server.c routing.c
	${CC} ${CFLAGS} -o backend server.c routing.c queue.c  

test: queue.c
	${CC} ${CFLAGS} -o test queue.c

clean:
	rm -rf *.dSYM backend
