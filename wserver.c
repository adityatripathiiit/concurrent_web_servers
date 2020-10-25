#include <stdio.h>
#include <pthread.h>
#include "request.h"
#include "io_helper.h"
#include <stdbool.h>
#include "common_threads.h"
#include <string.h>
int BUFSIZE = 1;
char default_root[] = ".";

char* SCHEDULING = "FIFO";

pthread_mutex_t lock;
pthread_cond_t full;
pthread_cond_t empty;


void init(){
	Pthread_cond_init(&full, NULL);
	Pthread_cond_init(&empty, NULL);
	Pthread_mutex_init(&lock, NULL)
}

int * BUFFER ; 
volatile int fill = 0;
volatile int count = 0;
volatile int use = 0;

//
// ./wserver [-d <basedir>] [-p <portnum>] [-t <threads>] [-b <MAX Threads>]
// 

void* thread_worker(void* arg)
{

	while(true)
	{
		Pthread_mutex_lock(&lock); 
		while(count == 0){
			printf("going to sleep\n"); 
			Pthread_cond_wait(&empty, &lock); 
			printf("awake\n"); 
		}
		int conn_fd  = BUFFER[use];
		use = (use+1)%BUFSIZE;
		count--;

		Pthread_cond_signal(&full);
		Pthread_mutex_unlock(&lock);	


		request_handle(conn_fd);
		close_or_die(conn_fd);
	}
	return NULL;
}


int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
	init();
    int port = 10000;
	int threads; 
    while ((c = getopt(argc, argv, "d:p:t:b:")) != -1)
	{
		switch (c) {
		case 'd':
			root_dir = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 't':
			threads = atoi(optarg);
			break;
		case 'b':
			BUFSIZE = atoi(optarg);
			break;
		default:
			fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b Buffer Size]\n");
			exit(1);
		}
	}

	BUFFER = malloc(BUFSIZE*sizeof(int));
	pthread_t * thread_queue = malloc(threads*sizeof(pthread_t)); 
    
	for (int i=0;i<threads;i++){
		Pthread_create(&thread_queue[i], NULL, thread_worker, NULL);
	}
	// run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);

	// int * new_conn; 
	struct sockaddr_in client_addr;
	int client_len ;
	int conn_fd ; 
    while (true) {
		client_len = sizeof(client_addr);
		conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		Pthread_mutex_lock(&lock); 
		while(count == BUFSIZE)
		{
			Pthread_cond_wait(&full,&lock);
		}	
		// Pthread_mutex_unlock(&lock);
		// Pthread_mutex_lock(&lock);
		BUFFER[fill] = conn_fd;
		fill = (fill+1)%BUFSIZE;
		count++;
		Pthread_cond_signal(&empty); 
		Pthread_mutex_unlock(&lock);
		
    }
    return 0;
}


    


 
