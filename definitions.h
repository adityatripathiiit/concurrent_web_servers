#ifndef __definitions_h__
#define __definitions_h__

// producer and consumer

void make_threads_and_buffers(int no_of_threads, int buffer_size, char *policy);
void put_in_pool(int conn_fd);
int get_from_pool();
void* thread_worker(void* arg);

// buffer

int is_buffer_full();
int is_buffer_empty();
void put_in_buffer(int conn_fd);
int get_from_buffer(); 

// different buffer implementations -

typedef struct __node_t {
    int fd;
    off_t parameter;
} node;


// heap (for SFF)

typedef struct __heap_t {
    int curr_size;
    int max_size;
    node* array;
} heap;


void _swap(node* x, node* y);
heap* init_heap(int heap_size);
void insert_in_heap(int conn_fd, off_t parameter, heap* Heap);
void heapify(heap* Heap, int index);
int extract_min(heap* Heap);


// circular queue (for FIFO)

typedef struct __queue_t {
    int max_size;
    int curr_size;
    int fill;
    int use;
    node* array;
} queue;


queue* init_queue(int queue_size);
void insert_in_queue(int conn_fd, queue* Queue);
int get_from_queue(queue* Queue);

typedef struct __thread_pool_t {

    int num_threads;
    int working_threads;
    pthread_t* pool;
    pthread_mutex_t LOCK;
    pthread_cond_t FILL;
    pthread_cond_t EMPTY;

} thread_pool;

typedef struct __scheduler_t {

    char* policy;
    int buffer_size;
    int curr_size;
    heap* Heap;
    queue* Queue;

} scheduler;

typedef struct __thread_arg {

    scheduler* d;
    thread_pool* workers;
    int num_request;

} thread_arg;


// helper functions

int get_file_name(int fd, char* filename);
off_t get_file_size(int fd);
int request_parse_uri_modified(char *uri, char *filename, char *cgiargs);

scheduler* init_scheduler(char* policy, int buffer_size);
thread_pool* init_thread_pool(int num_threads);
void start_threads(scheduler* d, thread_pool* workers);
void schedule_new_request(scheduler* d, int conn_fd);
int pick_request(scheduler* d);
int is_scheduler_full(scheduler* d);
int is_scheduler_empty(scheduler* d);
void give_to_scheduler(thread_pool* workers, scheduler* d, int conn_fd);
int get_from_scheduler(thread_pool* workers, scheduler* d);

#endif