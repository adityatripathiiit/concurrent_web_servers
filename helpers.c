#include "common_headers.h"

#define MAXBUF 16384

int request_parse_uri_modified(char *uri, char *filename, char *cgiargs) {
    char *ptr;    
    if (!strstr(uri, "cgi")) { 	            
        strcpy(cgiargs, "");
        snprintf(filename, MAXBUF, ".%s", uri);
        if (uri[strlen(uri)-1] == '/') {
            strcat(filename, "index.html");
        }
        return 1;
    } else { 
        // dynamic
        ptr = index(uri, '?');
        if (ptr) {
            strncpy(cgiargs, ptr+1, MAXBUF);
            *ptr = '\0';
        } else {
            strcpy(cgiargs, "");
        }        
        return 0;
    }
}


long requestFileSize(int fd) {
	char buf[MAXBUF], method[MAXBUF], uri[MAXBUF], version[MAXBUF];
    char filename[MAXBUF], cgiargs[MAXBUF];	
	struct stat s;
	recv(fd, buf, sizeof(buf), MSG_PEEK);
   	sscanf(buf, "%s %s %s\n", method, uri, version);
	request_parse_uri_modified(uri, filename, cgiargs);
	stat(filename, &s);

	return (long)s.st_size;
}