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

#define SOCK_ADDR struct sockaddr

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
    
    check(listen(sockfd,4));

    socklen_t cli_addr_size = sizeof(servaddr);
    conn_sock = check(accept(sockfd, &destaddr,&cli_addr_size));
 
    // int flags = fcntl(sockfd,F_GETFL,0);
    // if(0 > (flags)){
    //     perror("flags");
    //     exit(EXIT_FAILURE);  // 0 - success, -1 - fail
    // }
    // assert(flags != -1);
    // fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    printf("Connection from: %s \n",inet_ntoa(destaddr.sin_addr));

    while(conn_sock > 0){

        read(conn_sock , readline, BUFF_SIZE); 
        
        printf("%s\n",readline); 
        Request req = get_REST(readline);
        call_route(req.addr,conn_sock,req.request,req.data,req.length_data);
        close(conn_sock);
        conn_sock = accept(sockfd, &destaddr,&cli_addr_size);

    }
    
    printf("Connection closed: %s \n",inet_ntoa(destaddr.sin_addr));
 
}

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