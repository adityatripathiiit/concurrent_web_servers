#include "common_headers.h"
#include "definitions.h"

queue* init_queue(int queue_size){

    queue* newQueue = (queue*)malloc(sizeof(queue));
    if(newQueue==NULL) exit(1);    

    newQueue->curr_size = 0;
    newQueue->max_size = queue_size;
    newQueue->fill = 0;
    newQueue->use = 0;

    newQueue->array = (node*)malloc(sizeof(node)*queue_size);    

    if(newQueue->array == NULL) exit(1);

    for(int i=0; i < queue_size; i++){
        newQueue->array[i].fd = 0;
    }        

    return newQueue;
}


void insert_in_queue(int conn_fd, queue* Queue){

    assert(Queue->curr_size < Queue->max_size);

    Queue->array[Queue->fill].fd = conn_fd;
    Queue->fill = (Queue->fill + 1) % Queue->max_size;
    Queue->curr_size++;

}

int get_from_queue(queue* Queue){

    assert(Queue->curr_size > 0);

    int conn_fd = Queue->array[Queue->use].fd;
    Queue->use = (Queue->use + 1) % Queue->max_size;
    Queue->curr_size--;

    return conn_fd;
}