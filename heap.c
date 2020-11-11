#include "common_headers.h"
#include "definitions.h"

int heap_comparator(heap* Heap, int i, int j){

    // returns 1 iff i > j in comparison

    if(Heap->by_file_name==1){
        if(strcmp(Heap->array[i].file_name,Heap->array[j].file_name) > 0) return 1;
        return 0;
    } else {
        if(Heap->array[i].parameter > Heap->array[j].parameter) return 1;
        return 0;
    }    
}

void _swap(node* x, node* y){
    node temp = *x;
    *x = *y;
    *y = temp;
}

heap* init_heap(int heap_size, int by_file_name){

    heap* newHeap = (heap*)malloc(sizeof(heap));
    if(!newHeap) exit(1);
    newHeap->max_size = heap_size;
    newHeap->curr_size = 0;
    newHeap->by_file_name = by_file_name;
    newHeap->array = (node*)malloc(heap_size*sizeof(node));
    

    if(!newHeap->array) exit(1);

    for(int i=0;i< heap_size; i++){
        newHeap->array[i].fd = 0;
        newHeap->array[i].parameter = 0;
        newHeap->array[i].file_name = NULL;
    }

    return newHeap;
}

void insert_in_heap(int conn_fd, off_t parameter, char* file_name, heap* Heap){
    assert(Heap->curr_size < Heap->max_size);

    int index = Heap->curr_size;
    Heap->curr_size++;
    Heap->array[index].fd = conn_fd;
    Heap->array[index].parameter = parameter;     
    Heap->array[index].file_name = file_name;   
    
    while(index!=0 && (heap_comparator(Heap, index/2, index)==1)){
        _swap(&Heap->array[index/2], &Heap->array[index]);
        index = index/2;
    }
}

void heapify(heap* Heap, int index){

    int left = 2*index + 1;
    int right = 2*index + 2;

    int smallest = index;

    if(left < Heap->curr_size && (heap_comparator(Heap, smallest, left)==1)){
        smallest = left;
    }

    if(right < Heap->curr_size && (heap_comparator(Heap, smallest, right)==1)){
        smallest = right;
    }

    if(smallest != index){
        _swap(&Heap->array[smallest], &Heap->array[index]);
        heapify(Heap, smallest);
    }

}

int extract_min(heap* Heap){

    assert(Heap->curr_size > 0);
    
    _swap(&Heap->array[0], &Heap->array[Heap->curr_size-1]);
    int fd = Heap->array[Heap->curr_size-1].fd;    
    Heap->curr_size--;
    heapify(Heap, 0);
    return fd;
}
