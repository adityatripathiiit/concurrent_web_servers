#include "common_headers.h"


int request_parse_uri_modified(char *uri, char *filename, char *cgiargs) {
    char *ptr;    
    if (!strstr(uri, "cgi")) { 	            
        strcpy(cgiargs, "");
        sprintf(filename, ".%s", uri);
        if (uri[strlen(uri)-1] == '/') {
            strcat(filename, "index.html");
        }
        return 1;
    } else { 
        // dynamic
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        } else {
            strcpy(cgiargs, "");
        }        
        return 0;
    }
}


long requestFileSize(int fd) {
	char buf[8192], method[8192], uri[8192], version[8192];
    char filename[8192], cgiargs[8192];	
	struct stat s;
	recv(fd, buf, sizeof(buf), MSG_PEEK);
   	sscanf(buf, "%s %s %s\n", method, uri, version);
	request_parse_uri_modified(uri, filename, cgiargs);
	stat(filename, &s);

	return (long)s.st_size;
}