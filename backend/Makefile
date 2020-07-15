CC=gcc

CFLAGS= -g

all: backend 

backend: main.c server.c routing.c queue.c
	${CC} ${CFLAGS} -pthread -o backend main.c server.c routing.c queue.c  

# test: queue.c
# 	${CC} ${CFLAGS} -o test queue.c

clean:
	rm -rf *.dSYM backend
