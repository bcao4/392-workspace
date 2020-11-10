#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <process name>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int grep_to_parent[2], ps_to_grep[2];
    pipe(grep_to_parent);

    pid_t pID;
    if((pID = fork()) == 0) {
        pipe(ps_to_grep);
        if((pID = fork()) == 0) {
            close(ps_to_grep[0]);
            dup2(ps_to_grep[1], STDOUT_FILENO);
            execlp("ps", "ps", "-A", NULL);
        } else {
            int status;
            wait(&status);
            close(ps_to_grep[1]);
            dup2(ps_to_grep[0], STDIN_FILENO);
            close(grep_to_parent[0]);
            dup2(grep_to_parent[1], STDOUT_FILENO);
            execlp("grep", "grep", "-w", argv[1], NULL);
        }
    } else {
        int status;
        wait(&status);
        close(grep_to_parent[1]);
        dup2(grep_to_parent[0], STDIN_FILENO);

        char buffer[4096];
        while (1) {
            ssize_t count = read(grep_to_parent[0], buffer, sizeof(buffer));
            if(count == -1) {
                if(errno == EINTR) {
                    continue;
                } else {
                    perror("read()");
                    exit(EXIT_FAILURE);
                }
            } else if (count == 0) {
                break;
            } else {
                write(STDOUT_FILENO, buffer, count);
            }
        }
    }
    return EXIT_SUCCESS;
}