//
// client.c: A very, very primitive HTTP client.
// 
// To run, try: 
//      client hostname portnumber filename
//
// Sends one HTTP request to the specified HTTP server.
// Prints out the HTTP response.
//
// For testing your server, you will want to modify this client.  
// For example:
// You may want to make this multi-threaded so that you can 
// send many requests simultaneously to the server.
//
// You may also want to be able to request different URIs; 
// you may want to get more URIs from the command line 
// or read the list from a file. 
//
// When we test your server, we will be using modifications to this client.
//

#include "io_helper.h"
#include <pthread.h>
#include <sys/time.h> 
#include <unistd.h>
#define MAXBUF (8192)

volatile int testbit = 0;
//
// Send an HTTP request for the specified file 
//
void client_send(int fd, char *filename) {
    char buf[MAXBUF];
    char hostname[MAXBUF];
    
    gethostname_or_die(hostname, MAXBUF);
    
    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    write_or_die(fd, buf, strlen(buf));
}

//
// Read the HTTP response and print it out
//
void client_print(int fd) {
    char buf[MAXBUF];  
    int n;
    
    // Read and display the HTTP Header 
    n = readline_or_die(fd, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0)) {
        if(testbit == 0)
            printf("Header: %s", buf);
        
	n = readline_or_die(fd, buf, MAXBUF);
	
	// If you want to look for certain HTTP tags... 
	// int length = 0;
	//if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
	//    printf("Length = %d\n", length);
	//}
    }
    
    // Read and display the HTTP Body 
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
        if(testbit == 0)
            printf("Header: %s", buf);
            
	n = readline_or_die(fd, buf, MAXBUF);
    }
}

typedef struct _client_data {
    char *host;
    char *filename;
    int port;
} client_data;

void* single_client(void* arg) {
    
    client_data *t = (client_data*)(arg);
    char* host = t->host;
    char* filename = t->filename;
    int port = t->port;
    /* Open a single connection to the specified host and port */
    int clientfd = open_client_fd_or_die(host, port);
    
    client_send(clientfd, filename);
    client_print(clientfd);
    
    close_or_die(clientfd);
    return NULL;

}


int main(int argc, char *argv[]) {
    char *host, *filename;
    int port;
    int clientfd;
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <host> <port> <testbit> <filename(s)> \n", argv[0]);
        exit(1);
    }
    if(atoi(argv[3]) == 1){
        #define TESTMODE 
    }

    host = argv[1];
    port = atoi(argv[2]);
    testbit = atoi(argv[3]);
    int concur_clients = argc - 4;
    pthread_t threads[concur_clients];

    #ifdef TESTMODE
        char path[1024] = "./benchmarking/data/concurr_test_sff.csv";

        if (concur_clients == 1){
            remove(path);   
        }
    
        FILE * f = fopen(path, "a");

        if(f == NULL){
            printf("Could not create the CSV file %s\n", path);
            return 0;
        }
        struct timeval start, end; 
        gettimeofday(&start, NULL);
    #endif
    
    for(int i = 0; i < concur_clients; i++) {
        
        client_data *d = malloc(sizeof(client_data));
        d->host = host;
        d->port = port;
        d->filename = argv[4 + i];
        pthread_create(&threads[i], NULL, single_client, (void *)d);
        
    }
    for(int i = 0; i < concur_clients; i++) {
        pthread_join(threads[i], NULL);
    }

    #ifdef TESTMODE
        gettimeofday(&end, NULL);
        double time_taken; 
        time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
        time_taken = (time_taken + (end.tv_usec -start.tv_usec)) * 1e-6; 
        fprintf(f,"%d,%f\n",concur_clients, time_taken);
        fclose(f);
    #endif
    
    exit(0);
}