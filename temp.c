#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* go(void *args){
    printf("I am thread \n");
}

int main(){
    pthread_t* p;
    int c = 5;
    p = malloc(c*sizeof(pthread_t));
    for(int i=0;i<c;i++) pthread_create(&p[i], NULL, go, NULL);
    for(int i=0;i<c;i++) pthread_join(p[i], NULL);

    return 0;
}