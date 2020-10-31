#include "common_headers.h"
#include "definitions.h"

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

int get_file_name(int fd, char* filename){	
	char buf[8192], cgiargs[8192], uri[8192], method[8192], version[8192];        
	readline_or_die(fd, buf, 8192);
    fseek(fd, 0, SEEK_SET);
	sscanf(buf, "%s %s %s", method, uri, version);	
    printf("D - %s %s %s \n", method, uri, version);
	int is_static = request_parse_uri_modified(uri, filename, cgiargs);	
    return is_static;	
}


off_t get_file_size(int fd){    
    char filename[8192];
    off_t file_size;     
    int new_fd = dup(fd);
    if(new_fd < 0) exit(1);
    int is_static = get_file_name(new_fd, filename);        
    close(new_fd);
    if(is_static){
        struct stat sbuf;
        printf("%s \n", filename);
        if(stat(filename,&sbuf) < 0){
            file_size = 0;
        } else  file_size = sbuf.st_size;    
        return file_size;   
    } else {
        return 0;
    }    
}