#include "routing.h"

Route bad_route = {.empty=-1, .request=GET , .addr="", .fnc_ptr=bad_request};
Route route_root = {.empty=0, .request=GET , .addr="/", .fnc_ptr=root_response};
Route route_js = {.empty=0,  .request=GET , .addr="/script.js", .fnc_ptr=js_response};
Route route_css = {.empty=0, .request=GET , .addr="/style.css", .fnc_ptr=css_response};
Route route_png = {.empty=0, .request=GET ,.addr="/water.png", .fnc_ptr=png_response};
Route route_post_data = {.empty=0, .request=GET , .addr="/ReadDev", .fnc_ptr=get_data};
Route route_get_data = {.empty=0, .request=POST , .addr="/ReadDev", .fnc_ptr=post_data};


void bad_request(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    printf("BAD REQUEST!:\n");
    response(sockfd,"404 NOT FOUND","",0,"text/html");
}

void root_response(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    uint8_t fd = open("../www/index.html",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    printf("ROOT:\n");
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/html"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void js_response(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    uint8_t fd = open("../www/script.js",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/javascript"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void css_response(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    uint8_t fd = open("../www/style.css",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/css"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void png_response(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
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


void get_data(uint8_t sockfd,uint8_t request,uint8_t* request_content,size_t length_data){
    if(QUEUE_NOT_INITED == queue_if_init(&data_queue)){
        queue_init(&data_queue,200);
    }
    // get dev data
    bzero(request_content,length_data);
    queue_pull(request_content,&length_data,&data_queue);

    response(sockfd,"200 OK",request_content,length_data,"text");
    
}

void post_data(uint8_t sockfd,uint8_t request,uint8_t* request_content,size_t length_data){
    if(QUEUE_NOT_INITED == queue_if_init(&data_queue)){
        queue_init(&data_queue,200);
    }
    
    printf("post data: %s\n\n",request_content);
    queue_push(request_content,length_data,&data_queue);
    
    response(sockfd,"200 OK","",0,"text");
}

/*                                    LIBRARY PART                                                  */

Request get_REST(char* ptr){
    Request req;
    
    req.data=malloc(strlen(ptr));
    strncpy(req.data,ptr,strlen(ptr));
    req.length_data=strlen(ptr);

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

void init_routes(Route *init){
    for(uint8_t i = 0; i < MAX_ROUTES; i++){
        routes[i]=init;
    }

}

uint8_t key(uint8_t request,char* addr){
    size_t len = strlen(addr)+1; // +1 because REST request has 1 byte and is added to key 
    uint8_t buff[len],hash;
    snprintf(buff,len,"%d:%s",request,addr);
    hash=key2hash(buff,len);
    return hash;
}

uint8_t key2hash(char* input,size_t len){
    uint8_t hash = 0;
    for(uint8_t n = 0; n < len-1; n++){
        hash=hash+input[n];
        hash=hash*input[n];
    }
    hash=hash%MAX_ROUTES;
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
    uint8_t hash = key(new_route->request,new_route->addr);
    if(routes[hash]->empty == 0) return -1;
    routes[hash] = new_route;
    return 0;
};


uint8_t call_route(char* addr,uint8_t sockfd,uint8_t request,uint8_t* request_content, size_t length_data){
    uint8_t hash = key(request,addr);
    if(routes[hash]->fnc_ptr != NULL)routes[hash]->fnc_ptr(sockfd,request,request_content,length_data);
    free(request_content);
    return 0;
};
