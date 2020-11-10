/*******************************************************************************
 * Name        : spsort.c
 * Author      : Brandon Cao    
 * Date        : March 29, 2020
 * Description : Sorted Permission Finder.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <assert.h>  
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BUFF_SZ 1024*1024

int count_wc_l(int buffer_to_wc, int *total_line) {
    pid_t pid;
    char buffer[256] = {0};
    int wc_to_output[2];
    int count = 0;

    if(pipe(wc_to_output) < 0) {
        printf("Error: failed to fork %s", strerror(errno));
        return EXIT_FAILURE;
    }
    pid = fork();
    if (pid < 0) {
        printf("Error: failed to fork %s", strerror(errno));
        return EXIT_FAILURE;
    }
    if(pid == 0) {
        // Child process 
        close(wc_to_output[0]);
        // input to wc from pipe buffer_to_wc[0]
        dup2(buffer_to_wc, STDIN_FILENO);
        // wc output to pipe wc_to_output[1]
        dup2(wc_to_output[1], STDOUT_FILENO);
        execlp("wc", "wc", "-l", NULL);
    }
    // wait for child process
    wait(NULL);
    close(wc_to_output[1]);
    close(buffer_to_wc);
  
    // read wc output from wc_to_output[0]
    dup2(wc_to_output[0], STDIN_FILENO);
    count = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (count == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }
    close(wc_to_output[0]);
    buffer[count] = '\0';
    if(total_line) {
        *total_line = atoi(buffer);
    }
    return 0;
}

int sort_to_stdout(int buffer_to_sort) {
    pid_t pid;
    int sort_to_output[2];
    ssize_t count = 0, read_sz = 0;
    char *p;
    int num_realloc = 1;
    char * data_buf;
    int buf_sz = BUFF_SZ;

    data_buf = (char *) malloc(BUFF_SZ);
    if(data_buf == NULL) {
        printf("Failed to malloc. memory %d - error: [%s]\n", BUFF_SZ, strerror(errno));
        return EXIT_FAILURE;
    }

    memset(data_buf, 0, BUFF_SZ);
    if(pipe(sort_to_output) < 0) {
        printf("Error: failed to fork %s", strerror(errno));
        return EXIT_FAILURE ;
    }

    pid = fork();
    if (pid < 0) {
        printf("Error: failed to fork %s", strerror(errno));
        return EXIT_FAILURE ;
    }

    if(pid == 0) {
        // Child process 
        close(sort_to_output[0]);
        // input to wc from pipe buffer_to_sort[0]
        dup2(buffer_to_sort, STDIN_FILENO);
        // wc output to pipe sort_to_output[1]
        dup2(sort_to_output[1], STDOUT_FILENO);
        execlp("sort", "sort", NULL);
    }
    // wait for child process
    close(sort_to_output[1]);
    close(buffer_to_sort);
  
    // read wc output from sort_to_output[0]
    dup2(sort_to_output[0], STDIN_FILENO);

    p = data_buf;
    while(1) {
        read_sz = read(STDIN_FILENO, p, 4096);
        if (read_sz > 0) {
            //printf("%s", p);
            write(STDOUT_FILENO, p, 4096);
            p = p + read_sz;
            count = count + read_sz;
            if(count + 4096 > buf_sz) {
                num_realloc++; 
                data_buf = (char *) realloc(data_buf, num_realloc * BUFF_SZ); 
                if(data_buf == NULL) {
                    printf("Error: failed to realloc %s\n", strerror(errno)); 
                    return EXIT_FAILURE; 
                }
                buf_sz = buf_sz + BUFF_SZ;
                p = data_buf + count;
                memset(p, 0, BUFF_SZ);
            }
        }
        else {
            break;
        }
    }
    close(sort_to_output[0]);

    if(data_buf) {
        free(data_buf);
    }
    if(count == 0) {
        return EXIT_FAILURE; 
    }
    return 0;
}

int process_pfind(int argc, char *argv[]) {
    pid_t pid;
    int pfind_to_buffer[2];

    //printf pfind results to stdout pipe[write], then read stdin pipe[read] to buffer
    if(pipe(pfind_to_buffer) < 0) {
        printf("Error: failed to pipe %s", strerror(errno));
        return EXIT_FAILURE;
    }

    pid = fork();
    if(pid < 0) {
        printf("Error: failed to fork %s", strerror(errno));
        return EXIT_FAILURE;
    }
    if(pid == 0) {
        // Child process
        close(pfind_to_buffer[0]);
        dup2(pfind_to_buffer[1], STDOUT_FILENO);
        // all ps -A output to pipe pfind_to_buffer[1]
        execlp("./pfind", "./pfind", argv[1], argv[2], argv[3], argv[4], NULL);
    }
    close(pfind_to_buffer[1]);
    return pfind_to_buffer[0];
}

int process_sort(int argc, char *argv[]) {
    int fd_stdin;
    fd_stdin = process_pfind( argc, argv );
    dup2(fd_stdin, STDIN_FILENO);
    return sort_to_stdout(fd_stdin);
}

int process_wc(int argc, char *argv[]) {
    int fd_stdin;
    int total_line = 0;
    fd_stdin = process_pfind( argc, argv );
    dup2(fd_stdin, STDIN_FILENO);
    count_wc_l(fd_stdin, &total_line);
    close(fd_stdin);
    //printf("Total matches: %d\n", total_line);
    return total_line;
}

// ps -A | wc -l 
int main(int argc, char *argv[]) {

    int total_line;

    struct stat pfile;
    if(stat("pfind", &pfile) != 0) {
        printf("Error: pfind failed.\n");
        return EXIT_FAILURE;
    }

    if(stat("/usr/bin/sort", &pfile) != 0) {
        printf("Error: sort failed.\n");
        return EXIT_FAILURE;
    }

    if(argc != 5) {
        execlp("./pfind", "./spfind", "-h", NULL);
    }
    for (int i = 1; i < argc; ++i) {
        if(memcmp(argv[i],"-h", 2) == 0) {
            execlp("./pfind", "pfind", "-h", NULL);
        }
    }

    total_line = process_wc(argc, argv);
    
    if(total_line > 0 && process_sort(argc, argv) != 0) {
        return EXIT_FAILURE;
    } else {
        printf("Total matches: %d\n", total_line);
    }

    return 0;
}