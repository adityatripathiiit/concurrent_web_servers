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

// heap (for SFF)

typedef struct __heap_node_t {
    int fd;
    int parameter;
} heap_node;

typedef struct __heap_t {
    int curr_size;
    int max_size;
    heap_node* array;
} heap;


void _swap(heap_node* x, heap_node* y);
heap* init_heap(int heap_size);
void insert_in_heap(int conn_fd, int parameter, heap* Heap);
void heapify(heap* Heap, int index);
int extract_min(heap* Heap);



// circular queue (for FIFO)

typedef struct __queue_node_t {
    int fd;
} queue_node;

typedef struct __queue_t {
    int max_size;
    int curr_size;
    int fill;
    int use;
    queue_node* array;
} queue;


queue* init_queue(int queue_size);
void insert_in_queue(int conn_fd, queue* Queue);
int get_from_queue(queue* Queue);


// helper functions

int get_file_name(int fd, char* filename);
off_t get_file_size(int fd);
int request_parse_uri_modified(char *uri, char *filename, char *cgiargs);

#endif