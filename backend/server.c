#include "server.h"

//************************************************* THREAD PART *********************************************************//

pthread_t thread_pool[THREADS_POOL];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
Queue threads_tasks;

void conn_hnld(int socket){
    uint8_t readline[BUFF_SIZE];

    read(socket , readline, BUFF_SIZE); 
    printf("\n\n REQUEST: \n\n %s \n\n END OF REQUEST for socket: %d \n\n",readline,socket);
    
    Request req = get_REST(socket,readline);
    
    call_route(&req);
    bzero(readline,BUFF_SIZE);
    close(socket);
}

void * _thread(){
    int socket;
    size_t size;
    int8_t status_code;

    while(1){
        pthread_mutex_lock(&mutex);
        status_code=queue_pull(&socket,&size,&threads_tasks);
        if(QUEUE_EMPTY == status_code){
            pthread_cond_wait(&cond,&mutex);
            status_code=queue_pull(&socket,&size,&threads_tasks);
        }
        pthread_mutex_unlock(&mutex);
        
        conn_hnld(socket);        
    }

}

uint32_t check(uint32_t err_code){ // if success -> check is transparent
    if(err_code < 0){
        printf("Failure");
        exit(EXIT_FAILURE);
    };
    return err_code;
}

int websock(){

    int sockfd, opt=1;
    
    struct sockaddr_in servaddr;
    struct sockaddr_in destaddr;

    // char sendline[BUFF_SIZE];

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

    printf("Connection from: %s \n",inet_ntoa(destaddr.sin_addr));
   
    // Setting up threads pool
    check(queue_init(&threads_tasks,1000));
    
    for(int n =0; n <THREADS_POOL;n++){
        pthread_create(&thread_pool[n],NULL,_thread,NULL);
    }


    while(1){
        int conn_sock;
        conn_sock = accept(sockfd, &destaddr,&cli_addr_size);
        pthread_mutex_lock(&mutex);
        queue_push(&conn_sock,sizeof(int),&threads_tasks);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
   
    }
    
}
