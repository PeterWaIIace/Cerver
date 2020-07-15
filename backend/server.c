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

#define PORT 8080

#define BUFF_SIZE 4096

#define CONNECTIONS 100

#define SOCK_ADDR struct sockaddr

void * conn_hnld(call_args *thread_args);

uint32_t check(uint32_t err_code){ // if success -> check is transparent
    if(err_code < 0){
        printf("Failure");
        exit(EXIT_FAILURE);
    };
    return err_code;
}

int websock(){

    int sockfd, conn_sock, opt=1;
    struct sockaddr_in servaddr;
    struct sockaddr_in destaddr;

    char sendline[BUFF_SIZE];
    char readline[BUFF_SIZE];
    //************************************ Socket init ********************************************//

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY; 

    check(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR ,&opt, sizeof(opt)));

    check(bind(sockfd,&servaddr,sizeof(servaddr)));
    
    check(listen(sockfd,CONNECTIONS));

    socklen_t cli_addr_size = sizeof(servaddr);
    // conn_sock = check(accept(sockfd, &destaddr,&cli_addr_size));

 
    printf("Connection from: %s \n",inet_ntoa(destaddr.sin_addr));

    while(1){

        conn_sock = accept(sockfd, &destaddr,&cli_addr_size);
        read(conn_sock , readline, BUFF_SIZE); 
        
        printf("%s\n",readline); 
        Request req = get_REST(readline);
        
        // call_route(req.addr,conn_sock,req.request,req.data,req.length_data);
        pthread_t p;

        call_args *thread_args = malloc(sizeof(call_args));
        thread_args->data = malloc(req.length_data);
        thread_args->addr = malloc(req.length_addr);
        
        thread_args->sconn = conn_sock;
        thread_args->request = req.request;
        memcpy(thread_args->addr,req.addr,req.length_addr);
        memcpy(thread_args->data,req.data,req.length_data);
        
        thread_args->length_addr=req.length_addr;
        thread_args->length_data=req.length_data;
        
        pthread_create(&p,NULL,conn_hnld,thread_args);

    }
    
    printf("Connection closed: %s \n",inet_ntoa(destaddr.sin_addr));
 
}

void * conn_hnld(call_args *thread_args){
    printf("\n\nhere\n\n'");
    call_route(thread_args);
    close(thread_args->sconn);
    free(thread_args->data);
    free(thread_args->addr);
    free(thread_args);
};

int main(int argc, char **argv){
    // clock_t start, end;
    // int sockfd, n,new_socket,opt=1; 
    // int sendbytes;

    //************************************* Routes **********************************************//

    init_routes(&bad_route);
    add_route(&route_root);
    add_route(&route_js);
    add_route(&route_css);  
    add_route(&route_png); 
    add_route(&route_post_data);
    add_route(&route_get_data);

    websock();      
    
    return 0; 
}