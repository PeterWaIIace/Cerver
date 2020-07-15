#ifndef __ROUTING_H__
#define __ROUTING_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

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
#include "queue.h"

#define MAX_ROUTES 200
#define MAX_ROUTE_LENGTH 30
#define MAX_HEADER_LENGTH 300
#define MAX_REST_CMD_LENGTH 10

#define FOREACH_REQ(REQ) \
        REQ(GET)   \
        REQ(POST)  \
        REQ(PUT)   \
        REQ(PATCH)  \
        REQ(DELETE)  \

#define GENERATE_ENUM(ENUM) ENUM,
#define ENUM2STR(STRING) #STRING,

enum REST { FOREACH_REQ(GENERATE_ENUM) };

static const char *REST_STRING[] = { FOREACH_REQ(ENUM2STR) };

struct Request{
    uint8_t sconn;
    uint8_t request;
    size_t length_addr;
    size_t length_data;
    char * addr; 
    char * data;
}typedef Request;

struct Route{
    int8_t empty; 
    uint8_t request;
    char addr[MAX_ROUTE_LENGTH];
    void (*fnc_ptr)(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data); 
}typedef Route;

Route *routes[MAX_ROUTES];

Request get_REST(uint8_t sock, char* ptr);

void bad_request(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void root_response(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void js_response(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void css_response(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void png_response(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void get_data(uint8_t sockfd,uint8_t request,uint8_t* request_content,size_t length_data);
void post_data(uint8_t sockfd,uint8_t request,uint8_t* request_content,size_t length_data);
Queue* data_queue;

void init_routes(Route *init);
uint8_t add_route(Route* new_route);
uint8_t call_route(Request* req);
uint8_t response(uint8_t sockfd, char* code, char* content, size_t content_size,char* content_type);
uint8_t key(uint8_t request,char* addr);
uint8_t key2hash(char* input,size_t len);

extern Route bad_route;
extern Route route_root;
extern Route route_js;
extern Route route_css;
extern Route route_png;
extern Route route_post_data;
extern Route route_get_data;

#endif