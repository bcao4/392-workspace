#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 128 //Not including \0

void display_usage(char *progname) {
    printf("%s <filename>\n", progname);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        display_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int src_fd;    
    if ((src_fd = open(argv[1], O_RDONLY)) == -1) {        
    	fprintf(stderr, "Error: Cannot open source file '%s'. %s.\n",                
    			argv[1], strerror(errno));        
    	return EXIT_FAILURE;    
    }

    char *buf; 
    if((buf = malloc(sizeof(char) * (BUFSIZE + 1))) == NULL) {
        close(src_fd);
        return EXIT_FAILURE;
    }

    int bytes_read;
    while((bytes_read = read(src_fd, buf, BUFSIZE)) > 0) {
        buf[bytes_read] = '\0';
        printf("%s", buf);
    }

    if(bytes_read == -1)  {
        fprintf(stderr, "Failed to read from file '%s'. %s.\n", argv[1], strerror(errno));
    }

    free(buf);
    close(src_fd);
    return EXIT_SUCCESS;
}
