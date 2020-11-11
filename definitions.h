#ifndef __definitions_h__
#define __definitions_h__

#define MAXBUF 16384
#define MAXFILETYPE 8192


// different buffer implementations -

typedef struct __node_t {
    int fd;
    off_t parameter;
    char* file_name;    
} node;


// heap (for SFF)

typedef struct __heap_t {
    int curr_size;
    int max_size;
    int by_file_name;
    node* array;
} heap;


void _swap(node* x, node* y);
heap* init_heap(int heap_size, int by_file_name);
void insert_in_heap(int conn_fd, off_t parameter, char* file_name, heap* Heap);
void heapify(heap* Heap, int index);
int extract_min(heap* Heap);
int heap_comparator(heap* Heap, int i, int j);


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

    scheduler* scheduler;
    thread_pool* workers;
    int num_request;

} thread_arg;

// helper structs

typedef struct __file_prop_t {
    char* file_name;
    off_t file_size;
} file_prop;


// helper functions

int get_file_name(int fd, char* filename);
off_t get_file_size(int fd);
int request_parse_uri_modified(char *uri, char *filename, char *cgiargs);
file_prop* request_file_properties(int fd);

scheduler* init_scheduler(char* policy, int buffer_size);
thread_pool* init_thread_pool(int num_threads);
void start_threads(scheduler* d, thread_pool* workers);
void schedule_new_request(scheduler* d, int conn_fd);
int pick_request(scheduler* d);
int is_scheduler_full(scheduler* d);
int is_scheduler_empty(scheduler* d);
void give_to_scheduler(thread_pool* workers, scheduler* d, int conn_fd);
int get_from_scheduler(thread_pool* workers, scheduler* d);

void* thread_worker(void* arg);

#endif