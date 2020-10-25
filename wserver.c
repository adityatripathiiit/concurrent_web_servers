#include <stdio.h>
#include <pthread.h>
#include "request.h"
#include "io_helper.h"
#include <stdbool.h>
#include "common_threads.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int BUFSIZE = 1;
char default_root[] = ".";

char* SCHEDULING = "FIFO";

pthread_mutex_t lock;
pthread_cond_t full;
pthread_cond_t empty;

struct stat new_sff, old_sff, check ;

struct data{
	int fd;
	off_t size;
	
};

void init(){
	Pthread_cond_init(&full, NULL);
	Pthread_cond_init(&empty, NULL);
	Pthread_mutex_init(&lock, NULL); 
}

struct data * BUFFER ; 
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

		// struct data t = BUFFER[use];
		int conn_fd  = BUFFER[use].fd;
		printf("%ld \n", BUFFER[use].size); 
		// fstat(conn_fd, &check);
		// printf("Size of file %ld", check.st_size);

		// printf("%d\n", conn_fd); 
		use = (use+1)%BUFSIZE;
		count--;
		

		Pthread_cond_signal(&full);
		Pthread_mutex_unlock(&lock);	


		request_handle(conn_fd);
		close_or_die(conn_fd);
	}
	return NULL;
}

void getfilename(int fd, char* filename)
{
	// struct stat sbuf;
	char buf[8192], cgiargs[8192], uri[8192], method[8192], version[8192];
	readline_or_die(fd, buf, 8192);
	sscanf(buf, "%s %s %s", method, uri, version);
	// printf("%s\n", uri);

	int is_static = request_parse_uri(uri, filename, cgiargs);
	// printf("%s\n", filename);
	
}


int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
	init();
    int port = 10000;
	int threads; 
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
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
		case 's':

			if(strcmp("FIFO",optarg) == 0) SCHEDULING = "FIFO"; 
			else if(strcmp("SFF",optarg) == 0) SCHEDULING = "SFF"; 
			else {
				fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b Buffer Size] [-s FIFO or SFF ]\n");
				exit(1);	
			}
			break ;

		default:
			fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b Buffer Size] [-s FIFO or SFF ]\n");
			exit(1);
		}
	}

	BUFFER = (struct data* )malloc(BUFSIZE*sizeof(struct data));
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
		int* new_fd = malloc(sizeof(int));
		*new_fd = conn_fd;

		char filename[8192];
		off_t file_size; 
		if(strcmp(SCHEDULING, "FIFO") != 0)
		{
			getfilename(*new_fd, filename);
			struct stat sbuf;
			stat(filename,&sbuf);
			file_size = sbuf.st_size;
			
			// printf("%ld \n", file_size); 
		}
		
		
		// printf("%d\n", conn_fd); 	

		Pthread_mutex_lock(&lock); 
		while(count == BUFSIZE)
		{
			Pthread_cond_wait(&full,&lock);
		}	
		// Pthread_mutex_unlock(&lock);
		// Pthread_mutex_lock(&lock);
		if(strcmp("FIFO", SCHEDULING) == 0){
			struct data t;
			t.fd = conn_fd;
			t.size = NULL;
			BUFFER[fill] = t;
			fill = (fill+1)%BUFSIZE;
			count++;
		}
		else if(strcmp("SFF", SCHEDULING) == 0){
			struct data t;
			t.fd = conn_fd;
			t.size = file_size;
			if(fill == use){	
				BUFFER[fill] = t;
				fill = (fill+1)%BUFSIZE;
				count++;
			}
			else {
				
				// off_t new_size = new_sff.st_size;
				// off_t old_size = old_sff.st_size;

				if(file_size < BUFFER[use].size){
					use = (use - 1)%BUFSIZE;
					// struct data t;
					// t.fd = conn_fd;
					// t.size = file_size;
					BUFFER[use] = t;
					count ++; 
				}
				
				else {
					int temp_count = use ; 
					// fstat(BUFFER[temp_count],&old_sff);
					// old_size = old_sff.st_size ;
					while(file_size>BUFFER[temp_count].size){
						temp_count = (temp_count+1)%BUFSIZE;
					}
					
					struct data swap = BUFFER[temp_count]; 
					BUFFER[temp_count] = t;
					
					temp_count = (temp_count+1)%BUFSIZE; 
					while(temp_count != fill){

						struct data temp = BUFFER[temp_count];
						
						BUFFER[temp_count] = swap;
						swap = temp; 
						temp_count = (temp_count+1)%BUFSIZE; 
					}
					BUFFER[fill] = swap;
					fill++; 
					count++;
				}
			}
		}
		
		Pthread_cond_signal(&empty); 
		Pthread_mutex_unlock(&lock);
		
    }
    return 0;
}


    


 
