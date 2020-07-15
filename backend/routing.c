#include "routing.h"

Route bad_route = {.request=GET , .addr=EMPTY_ROUTE, .fnc_ptr=bad_request}; // init route_css

void bad_request(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    printf("BAD REQUEST!:\n");
    response(sockfd,"404 NOT FOUND","",0,"text/html");
}

/*                                    LIBRARY PART                                                  */

Request get_REST(uint8_t sock, char* ptr){
    Request req;
    req.sconn = sock;
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
    req.addr = strtok(NULL," ");
    printf("here %s %d\n\n",req.addr,strlen(req.addr));
    req.length_addr = strlen(req.addr);
    return req;
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
    printf("START OF RESPONSE \n%s\nEND OF RESPONSE socket %d \n\n",strsend,sockfd);

    write(sockfd , (uint8_t*)strsend, strlen(header)+content_size);
}


void init_routes(){

    for(uint8_t i = 0; i < MAX_ROUTES; i++){
       
        routes[i]=&bad_route;
        
    }
}

uint8_t add_route(Route* new_route){
    if(NULL == new_route) return ROUTE_EXISTS;
    uint8_t hash = key(new_route->request,new_route->addr);
    
    if(0!=strncmp(routes[hash]->addr,EMPTY_ROUTE,strlen(EMPTY_ROUTE))){ // if hash is occupied find new one
        for(uint8_t i =0;i < MAX_ROUTES; i++){
            uint8_t try = (hash + i) % MAX_ROUTES;
            if(0==strncmp(routes[try]->addr,EMPTY_ROUTE,strlen(EMPTY_ROUTE)))  
                    routes[try] = new_route;
                    return ROUTE_IS_FREE;
        }
        return ROUTE_EXISTS; // if all hashes are occupied - nothing to find
    }
    else{
        routes[hash] = new_route;
        return ROUTE_IS_FREE;
    }
    
};

uint8_t call_route(Request* req){
    uint8_t sconn,request;
    uint8_t *request_content;
    size_t length_data;
    request_content = malloc(req->length_data);
    length_data=req->length_data;
    bzero(request_content,length_data);
    
    sconn=req->sconn;
    request=req->request; 
    
    printf("addr %s %d\n\n",req->addr,req->length_addr);
    memcpy(request_content,req->data,req->length_data);
    
    uint8_t call_bad_route = 1;
    
    uint8_t hash = key(request,req->addr);    
    if(0==strncmp(routes[hash]->addr,req->addr,strlen(req->addr))){ // if hash is occupied find new one
        if(routes[hash]->fnc_ptr != NULL)routes[hash]->fnc_ptr(sconn,request,request_content,length_data);
        call_bad_route = 0;
    }
    
    else{
        for(uint8_t i =0;i < MAX_ROUTES; i++){
            uint8_t try = (hash + i) % MAX_ROUTES;
            if(0==strncmp(routes[try]->addr,req->addr,strlen(req->addr))){
                if(routes[hash]->fnc_ptr != NULL)routes[hash]->fnc_ptr(sconn,request,request_content,length_data);
                call_bad_route = 0;
            }
        }
    }
    if(1 == call_bad_route)bad_route.fnc_ptr(sconn,request,request_content,length_data); // if hash doesnt lead to any route call bad route
    free(request_content);
    return 0;
}
