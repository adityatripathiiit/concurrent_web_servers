#include "common_headers.h"
#include "definitions.h"

pthread_t *threads = NULL;
heap* Heap = NULL;
queue* Queue = NULL;
pthread_mutex_t LOCK;
pthread_cond_t FILL;
pthread_cond_t EMPTY;
char* scheduling_policy = "SFF";

void make_threads_and_buffers(int no_of_threads, int buffer_size, char *policy){

    threads = malloc(no_of_threads*sizeof(pthread_t));  

    scheduling_policy = policy;    

    Pthread_mutex_init(&LOCK, NULL);
    Pthread_cond_init(&FILL, NULL);
    Pthread_cond_init(&EMPTY, NULL);    

    if(strcmp("SFF", scheduling_policy)==0){        
        Heap = init_heap(buffer_size);        
    } else if(strcmp("FIFO", scheduling_policy)==0){        
        Queue = init_queue(buffer_size);        
    }    

    for (int i=0;i<no_of_threads;i++){        
		pthread_create(&threads[i], NULL, thread_worker, &i);
	}    
}

void put_in_pool(int conn_fd){

    // each request tries to fill the pool (Producer)

    Pthread_mutex_lock(&LOCK); 
    while(is_buffer_full()==1)
    {
        Pthread_cond_wait(&FILL, &LOCK);
    }
    put_in_buffer(conn_fd);
    Pthread_cond_signal(&EMPTY);
    Pthread_mutex_unlock(&LOCK);
}

int get_from_pool(){

    // each thread tries to get from pool (Consumer)     

    Pthread_mutex_lock(&LOCK);    
    while(is_buffer_empty()==1){        
        Pthread_cond_wait(&EMPTY, &LOCK);        
    }            
    int conn_fd = get_from_buffer();
    Pthread_cond_signal(&FILL);
    Pthread_mutex_unlock(&LOCK);

    return conn_fd;
}

int is_buffer_full(){        
    if(strcmp("SFF", scheduling_policy)==0) {
        if(Heap->max_size == Heap->curr_size) return 1;        
    } else if(strcmp("FIFO", scheduling_policy)==0){        
        if(Queue->max_size == Queue->curr_size) return 1;        
    }    
    return 0;
}


int is_buffer_empty(){           
    if(strcmp("SFF", scheduling_policy)==0) {  
        if(Heap->curr_size == 0) return 1;        
    } else if(strcmp("FIFO", scheduling_policy)==0){        
        if(Queue->curr_size == 0) return 1;        
    }    
    return 0;
}


void put_in_buffer(int conn_fd){    
    if(strcmp(scheduling_policy, "SFF")==0){
        int parameter = 10; //ISKO CHALADO KOI PLEASE //get_file_size(conn_fd);                                 
        insert_in_heap(conn_fd, parameter, Heap);        
    } else if(strcmp("FIFO", scheduling_policy)==0){
        insert_in_queue(conn_fd, Queue);
    }
}

int get_from_buffer(){
    int conn_fd;
    if(strcmp(scheduling_policy, "SFF")==0){
        conn_fd = extract_min(Heap);        
    } else if(strcmp("FIFO", scheduling_policy)==0){
        conn_fd = get_from_queue(Queue);
    }
    return conn_fd;
}