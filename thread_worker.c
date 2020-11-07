#include "common_headers.h"
#include "definitions.h"

void* thread_worker(void* arg)
{	
	// int tn = *((int *)arg);
	thread_arg* args = (thread_arg*)arg;
	while(1)
	{			
		int conn_fd = get_from_scheduler(args->workers, args->d);		
		printf("thread = %d, conn_fd = %d\n", args->num_request, conn_fd);
		request_handle(conn_fd);
		close_or_die(conn_fd);
	}
	return NULL;
}