#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define QUEUE_INITED 0
#define QUEUE_NOT_INITED -1
#define BUFF_SIZE 4096

typedef struct element{
    size_t size;
    struct element* next; 
    uint8_t value[BUFF_SIZE];
}  element;

struct Queue{
    element* head;
    element* tail;
    uint16_t size;
    uint16_t MAX_SIZE;
    uint8_t init;
}typedef Queue;

;

int8_t queue_init(Queue *ptr,uint16_t max_size);
int8_t queue_if_init(Queue *ptr);
int8_t queue_size(Queue *ptr);
int8_t queue_push(uint8_t* buff,size_t size, Queue *ptr);
int8_t queue_pull(uint8_t* read_val, size_t* size, Queue *ptr);

#endif // __QUEUE_H__