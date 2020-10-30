#include "common_headers.h"
#include "definitions.h"

void* thread_worker(void* arg)
{	
	int tn = *((int *)arg);
	while(1)
	{			
		int conn_fd = get_from_pool();		
		printf("thread = %d, conn_fd = %d\n", tn, conn_fd);
		request_handle(conn_fd);
		close_or_die(conn_fd);
	}
	return NULL;
}