#include "common_headers.h"
#include "definitions.h"

scheduler* init_scheduler(char* policy, int buffer_size) {

    scheduler* d = (scheduler*)malloc(sizeof(scheduler));
    if(d == NULL) {
        return NULL;
    }
    d->policy = policy;
    d->buffer_size = buffer_size;
    d->curr_size = 0;

    // Decide the data structure based on scheduling policy
    if (strcmp("SFF", policy) == 0) {        
        d->Heap = init_heap(buffer_size, 0); 
        d->Queue = NULL;       
    } else if (strcmp("FIFO", policy) == 0) {        
        d->Queue = init_queue(buffer_size);        
        d->Heap = NULL;
    } else if(strcmp("SFNF", policy) == 0){
        d->Heap = init_heap(buffer_size, 1); 
        d->Queue = NULL;       
    }
    return d;  

}

thread_pool* init_thread_pool(int num_threads) {

    thread_pool* workers = (thread_pool*)malloc(sizeof(thread_pool));
    if(workers == NULL) {
        return NULL;
    }
    workers->num_threads = num_threads;
    workers->working_threads = 0;
    workers->pool = malloc(num_threads * sizeof(pthread_t));

    Pthread_mutex_init(&workers->LOCK, NULL);
    Pthread_cond_init(&workers->FILL, NULL);
    Pthread_cond_init(&workers->EMPTY, NULL);

    return workers;   

}

void start_threads(scheduler* d, thread_pool* workers) {
    for (int i = 0; i < workers->num_threads; i++) {
        thread_arg* arg = (thread_arg*)malloc(sizeof(thread_arg));
        if(arg == NULL) {
            printf("Thread number %d: Creation failed", i);
            continue;
        }
        arg->workers = workers;
        arg->scheduler = d;
        arg->num_request = i;
        Pthread_create(&workers->pool[i], NULL, thread_worker, arg);
    }
}

void schedule_new_request(scheduler* d, int conn_fd) {

    if (strcmp(d->policy, "SFF") == 0 || strcmp("SFNF", d->policy) == 0) {

        file_prop* fileProp = request_file_properties(conn_fd);           
        printf("File Size for %d : %ld and name %s\n", conn_fd,fileProp->file_size,fileProp->file_name);
        insert_in_heap(conn_fd, fileProp->file_size,fileProp->file_name,d->Heap);        
    
    } else if (strcmp("FIFO", d->policy) == 0) {

        insert_in_queue(conn_fd, d->Queue);

    } 
    d->curr_size++;

}

int pick_request(scheduler* d) {

    int conn_fd;
    if (strcmp(d->policy, "SFF") == 0 || strcmp("SFNF", d->policy) == 0) {
        conn_fd = extract_min(d->Heap);        
    } else if (strcmp("FIFO", d->policy) == 0) {
        conn_fd = get_from_queue(d->Queue);
    }
    d->curr_size--;
    return conn_fd;
}

int is_scheduler_full(scheduler* d) {
    return d->curr_size == d->buffer_size;
}

int is_scheduler_empty(scheduler* d) {
    return d->curr_size == 0;
}

void give_to_scheduler(thread_pool* workers, scheduler* d, int conn_fd) {

    Pthread_mutex_lock(&workers->LOCK); 
    while(is_scheduler_full(d)) {
        Pthread_cond_wait(&workers->FILL, &workers->LOCK);
    }
    schedule_new_request(d, conn_fd);
    Pthread_cond_signal(&workers->EMPTY);
    Pthread_mutex_unlock(&workers->LOCK);

}

int get_from_scheduler(thread_pool* workers, scheduler* d) {

    Pthread_mutex_lock(&workers->LOCK);    
    while(is_scheduler_empty(d)) {        
        Pthread_cond_wait(&workers->EMPTY, &workers->LOCK);        
    }
    int conn_fd = pick_request(d);

    printf("Request Scheduled for FD: %d\n", conn_fd);
    
    Pthread_cond_signal(&workers->FILL);
    Pthread_mutex_unlock(&workers->LOCK);

    return conn_fd;
}
