#include "common_headers.h"
#include "definitions.h"

void _swap(node* x, node* y){
    node temp = *x;
    *x = *y;
    *y = temp;
}

heap* init_heap(int heap_size){

    heap* newHeap = (heap*)malloc(sizeof(heap));
    if(!newHeap) exit(1);
    newHeap->max_size = heap_size;
    newHeap->curr_size = 0;
    newHeap->array = (node*)malloc(heap_size*sizeof(node));

    if(!newHeap->array) exit(1);

    for(int i=0;i< heap_size; i++){
        newHeap->array[i].fd = 0;
        newHeap->array[i].parameter = 0;
    }

    return newHeap;
}

void insert_in_heap(int conn_fd, off_t parameter, heap* Heap){
    assert(Heap->curr_size < Heap->max_size);

    int index = Heap->curr_size;
    Heap->curr_size++;
    Heap->array[index].fd = conn_fd;
    Heap->array[index].parameter = parameter;        
    
    while(index!=0 && (Heap->array[(index/2)].parameter > Heap->array[index].parameter)){
        _swap(&Heap->array[index/2], &Heap->array[index]);
        index = index/2;
    }
}

void heapify(heap* Heap, int index){

    int left = 2*index + 1;
    int right = 2*index + 2;

    int smallest = index;

    if(left < Heap->curr_size && Heap->array[left].parameter < Heap->array[smallest].parameter){
        smallest = left;
    }

    if(right < Heap->curr_size && Heap->array[right].parameter < Heap->array[smallest].parameter){
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
