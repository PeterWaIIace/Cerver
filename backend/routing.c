#include "routing.h"

Route bad_route = {.empty=-1,.addr="",.fnc_ptr=bad_request};
Route route_root = {.empty=0,.addr="/",.fnc_ptr=root_response};
Route route_js = {.empty=0,.addr="/script.js",.fnc_ptr=js_response};
Route route_css = {.empty=0,.addr="/style.css",.fnc_ptr=css_response};
Route route_png = {.empty=0,.addr="/water.png",.fnc_ptr=png_response};

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

void bad_request(uint8_t sockfd){
    response(sockfd,"404 NOT FOUND","",0,"text/html");
}

void root_response(uint8_t sockfd){
    uint8_t fd = open("../www/index.html",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/html"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void js_response(uint8_t sockfd){
    uint8_t fd = open("../www/script.js",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/javascript"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void css_response(uint8_t sockfd){
    uint8_t fd = open("../www/style.css",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/css"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void png_response(uint8_t sockfd){
    // uint8_t fd = open("../www/water.png",O_RDONLY,S_IRUSR | S_IWUSR);
    FILE *fpng = fopen("../www/water.png","rb");


    fseek(fpng, 0, SEEK_END);
    long fsize = ftell(fpng);
    fseek(fpng, 0, SEEK_SET);  /* same as rewind(f); */

    uint8_t *rpng = malloc(fsize + 1);
    fread(rpng, 1, fsize, fpng);
    fclose(fpng);

    // struct stat sb;

    // fstat(fd,&sb);
    
    // uint8_t* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    // response(sockfd,"200 OK",rfile,sb.st_size,"image/png"); 
    response(sockfd,"200 OK",rpng,fsize,"image/png"); 
    // munmap(rfile,sb.st_size);
    
};

void init_routes(Route *init){
    for(uint8_t i = 0; i < MAX_ROUTES; i++){
        routes[i]=init;
    }
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

uint8_t response(uint8_t sockfd, char* code, char* content, size_t content_size,char* content_type){
    uint8_t header[MAX_HEADER_LENGTH]; 
    snprintf(header,MAX_HEADER_LENGTH,"HTTP/1.1 %s \naccept-ranges: bytes\nContent-length: %d\nContent-Type: %s\n\n",code,content_size,content_type);
    uint8_t strsend[strlen(header)+content_size];
    bzero(strsend, strlen(header)+content_size);
    
    memcpy(strsend, header,strlen(header));
    memcpy(strsend+strlen(header), content,content_size);
    printf("%s\n",strsend);

    write(sockfd , (uint8_t*)strsend, strlen(header)+content_size);
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
