#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/un.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include <netdb.h>
#include <netinet/in.h> 

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#include <time.h>

#include <pthread.h>

#include "routing.h"
#include "queue.h"

#define PORT 8080

#define BUFF_SIZE 4096

#define CONNECTIONS 500
#define THREADS_POOL 10

#define SOCK_ADDR struct sockaddr

uint32_t check(uint32_t err_code);

void * _thread();
void conn_hnld(int socket);
int websock();

#endif // __SERVER_H__