#include "common_headers.h"
#include "common_threads.h"
#include "definitions.h"

char default_root[] = ".";

int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;	
    int port = 10000;
	int no_of_threads = 1; 
	int buffer_size = 1;
	char* scheduling_policy = "FIFO";
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
			no_of_threads = atoi(optarg);
			break;
		case 'b':
			buffer_size = atoi(optarg);
			break;
		case 's':
			if(strcmp("FIFO",optarg)==0) scheduling_policy = "FIFO";
			else if(strcmp("SFF",optarg)==0) scheduling_policy = "SFF";
			else {
				fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b Buffer Size] [-s SFF or FIFO]\n");
				exit(1);	
			}
			break;
		default:
			fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b Buffer Size] [-s SFF or FIFO]\n");
			exit(1);
		}
	}		

	// The scheduler is responsible maintaing the requests according to scheduling policy
	scheduler* scheduler = init_scheduler(scheduling_policy, buffer_size);

	// The thread_pool maintains all the threadsl, locks and condition variables
	thread_pool* workers = init_thread_pool(no_of_threads);

	// starting the threads
	start_threads(scheduler, workers);
	
	// run out of this directory
    chdir_or_die(root_dir);
    
    int listen_fd = open_listen_fd_or_die(port);	
	
	struct sockaddr_in client_addr;
	int client_len;
	int conn_fd; 
	
    while (1) {
		printf("Reached \n");
		client_len = sizeof(client_addr);
		conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		printf("Connected, FD: %d\n", conn_fd);
		
		// Schedule the current request
		give_to_scheduler(workers, scheduler, conn_fd);	
    }
    return 0;
}