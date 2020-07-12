#include "routing.h"

Route bad_route = {.empty=-1,.addr="",.fnc_ptr=bad_request};
Route route_root = {.empty=0,.addr="/",.fnc_ptr=root_response};
Route route_js = {.empty=0,.addr="script.js",.fnc_ptr=js_response};
Route route_css = {.empty=0,.addr="/style.css",.fnc_ptr=css_response};

Request get_REST(char* ptr){
    Request req;
    ptr = strtok(ptr," ");
    int n_req;
    for(n_req=GET;n_req<=DELETE;n_req++){
        if(0==strncmp(ptr,REST_STRING[n_req],sizeof(REST_STRING[n_req]))){
            break;
        }
    }
    req.request = n_req;
    req.addr = strtok(NULL, " ");
    req.length_addr = sizeof(req.addr);

    return req;
}

uint8_t key(char* addr){
    uint32_t hash = 0;
    for(uint8_t n = 0; n < strnlen(addr, MAX_ROUTE_LENGTH); n++){
        hash+=addr[n];
        hash*=addr[n];
    }
    hash%=MAX_ROUTES;
    return hash;
};

uint8_t response(uint8_t sockfd, char* code, char* content, size_t content_size){
    char header[MAX_HEADER_LENGTH]; 
    snprintf(header,MAX_HEADER_LENGTH,"HTTP/1.1 %s\nContent-length: %d\nContent-Type: text/html\n\n",code,content_size);
    char strsend[strlen(header)+content_size];
    bzero(strsend, strlen(header)+content_size);
    
    strcat(strsend, header);
    strcat(strsend, content);
    printf(strsend);
    write(sockfd , strsend, strlen(header)+content_size);
}

void bad_request(uint8_t sockfd){
    response(sockfd,"404 NOT FOUND","",0);
}

void root_response(uint8_t sockfd){
    uint8_t fd = open("../www/index.html",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void js_response(uint8_t sockfd){
    uint8_t fd = open("../www/script.js",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void css_response(uint8_t sockfd){
    uint8_t fd = open("../www/style.css",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size); 
    munmap(rfile,sb.st_size);
    close(fd);
};


void init_routes(Route *init){
    for(uint8_t i = 0; i < MAX_ROUTES; i++){
        routes[i]=init;
    }
}

uint8_t add_route(Route* new_route){
    if(new_route == NULL) return -1;
    uint8_t hash = key(new_route->addr);
    if(routes[hash]->empty == 0) return -1;
    routes[hash] = new_route;
    return 0;
};

uint8_t call_route(char* addr,uint8_t sockfd){
    uint8_t hash = key(addr);
    if(routes[hash]->fnc_ptr != NULL)routes[hash]->fnc_ptr(sockfd);
    return 0;
};
