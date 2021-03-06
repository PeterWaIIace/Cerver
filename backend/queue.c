#include "queue.h"

int8_t queue_init(Queue *ptr,uint16_t max_size){
    if(ptr == NULL) return -1;
    ptr->head = NULL;
    ptr->tail = NULL;
    ptr->size =  0;
    ptr->MAX_SIZE = max_size;
    ptr->init=1;
    return 0;
}

int8_t queue_if_init(Queue *ptr){
    if(1==ptr->init) return QUEUE_INITED;
    return QUEUE_NOT_INITED;
}

uint8_t queue_size(Queue *ptr){
    return ptr->size;
}

int8_t queue_push(uint8_t *buff, size_t size, Queue *ptr){
    if(ptr->size == ptr->MAX_SIZE){
        uint8_t trash;
        size_t trash_size;
        queue_pull(&trash,&trash_size,ptr);       
    }

    element *node=malloc(sizeof(node)); 
    memcpy(node->value,buff,size);
    node->size = size;

    if(ptr->head==NULL) ptr->head=node;
    if(NULL == ptr->tail) ptr->tail=node;
   
    ptr->head->next = node;
    ptr->head=node;
    ptr->size +=1;
    return 0;
}

int8_t queue_pull(uint8_t *buff, size_t* size, Queue *ptr){
    if(0 >= queue_size(ptr)){
        ptr->size=0;
        buff=NULL;
        size=0;
        return -1;
        }
    else{
        *size = ptr->tail->size;
        memcpy(buff,ptr->tail->value,ptr->tail->size);
        printf("socket in queue: %d\n\n",*buff);
        if(ptr->tail == ptr->tail->next){
            ptr->tail->next =NULL;
            ptr->head = NULL;
        }
        
        element *node;
        node = ptr->tail;
        ptr->tail = ptr->tail->next;
        ptr->size -=1;
        free(node);
        return 0;
    }
}

// int main(){
//     Queue Q1; 
//     char * dataone = "one1";
//     queue_init(&Q1,6);
//     queue_push(dataone,strlen(dataone),&Q1);
//     printf("push: %s\n",dataone);
//     queue_push("two2",strlen("two2"),&Q1);
//     printf("push: %s\n","two2");
//     queue_push("three3",strlen("three3"),&Q1);
//     printf("push: %s\n","three3");
//     queue_push("four4",strlen("four4"),&Q1);
//     printf("push: %s\n","four4");
//     queue_push("5fize",strlen("5fize"),&Q1);
//     printf("push: %s\n","5fize");
//     queue_push("LoL",strlen("LoL"),&Q1);
//     printf("push: %s\n","LoL");

//     uint8_t read_val[BUFF_SIZE];
//     int8_t n;
//     size_t size;
//     n=queue_pull(read_val,&size,&Q1);
//     printf("read: %s err_code: %d size: %d \n",read_val,n,strlen(read_val),size);
//     bzero(read_val,BUFF_SIZE);
//     n=queue_pull(read_val,&size,&Q1);
//     printf("read: %s err_code: %d size: %d \n",read_val,n,strlen(read_val),size);
//     bzero(read_val,BUFF_SIZE);
//     n=queue_pull(read_val,&size,&Q1);
//     printf("read: %s err_code: %d size: %d \n",read_val,n,strlen(read_val),size);
//     bzero(read_val,BUFF_SIZE);
//     n=queue_pull(read_val,&size,&Q1);
//     printf("read: %s err_code: %d size: %d \n",read_val,n,strlen(read_val),size);
//     bzero(read_val,BUFF_SIZE);
//     n=queue_pull(read_val,&size,&Q1);
//     printf("read: %s err_code: %d size: %d \n",read_val,n,strlen(read_val),size);
//     bzero(read_val,BUFF_SIZE);
//     return 0;

// }