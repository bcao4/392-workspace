/*******************************************************************************
 * Name        : minishell.c
 * Author      : Brandon Cao and Jerry Yu
 * Date        : 4/10/20
 * Description : minishell
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <unistd.h>

#define BUFSIZE 128
#define BRIGHTBLUE "\x1b[34;1m" 
#define DEFAULT    "\x1b[0m"
#define MAX_COMMAND 256
#define MAX_LEN 1024

sigjmp_buf jmpbuf;

typedef struct _cmd_t {
    int argc;
    char *argv[MAX_COMMAND];
} cmd_t;

void parse(char *input, cmd_t *command) {
    char *param;
    int arg_num = 0;

    param = strtok(input, " ");
    while(param != NULL) {
        command->argv[arg_num] = param;
        param = strtok(NULL, " ");
        arg_num++;
        if(arg_num >= MAX_COMMAND - 1) {
            break;
        }
    }
    command->argc = arg_num;
    command->argv[arg_num] = NULL;
    return;
}

/*
void remove_space(char input[], char buf[]) {
    int i = 0;
    int j = 0;
    bool space = false;

    while((input[i] = buf[j++]) != '\0') {
        if(input[i] == ' ') {
            if(space == true){ 
                continue;
            }
            space = true;
        } else {
            space = false;
        }
        i++;
    }
    if(space == true) {
        input[i-1] = '\0';
    } else {
        input[i] = '\0';
    }
}

void parse(char *input, cmd_t *command) {
    char *param;
    int arg_num = 0;

    param = strtok(input, " ");
    while(param != NULL) {
        command->argv[arg_num] = param;
        param = strtok(NULL, " ");
        arg_num++;
        if(arg_num >= MAX_COMMAND - 1) {
            break;
        }
    }
    command->argc = arg_num;
    command->argv[arg_num] = NULL;
    return;
}
*/

void catch_signal(int sig) {
    write(STDOUT_FILENO, "\n", 1);
    siglongjmp(jmpbuf, 1);
}

int execute(cmd_t *params) {
    pid_t child_pid;
    int child = 0;
    int status;

    child = fork();
    if(child == -1) {
        printf("Error: fork() failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if(child == 0) {
        if ((execvp(params->argv[0], params->argv)) == -1) {
            printf("Error: exec() failed. %s.\n", strerror(errno));
            exit(0);
        }
    } 
    while((child_pid = waitpid(-1, &status, WNOHANG))!= -1) {
        if(child_pid == -1 && errno != ECHILD) {
            printf("Error: wait() failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }
    return 0;
}

void cd_func(cmd_t *params) {
    char *current_dir;
    char cwd[MAX_LEN];
    current_dir = getcwd(cwd, sizeof(cwd));

    if (current_dir == NULL) {
        printf("Error: Cannot get current working directory. %s.\n", strerror(errno));
        //printf("argc: %i\n", params->argc);
        return;
    }
    else if (params->argc > 2) {
        printf("Error: Too many arguments to cd.\n");
        //printf("argc: %i\n", params->argc);
        return;
    }
    else if(params->argc == 1 || strcmp(params->argv[1], "~") == 0) {
        chdir(getenv("HOME"));
        //printf("argc: %i\n", params->argc);
    }
    else if(strcmp(params->argv[1], "-") == 0) {
        chdir(getenv("OLDPWD"));
        //printf("argc: %i\n", params->argc);
    }
    else if(params->argc == 2) {
        if(chdir(params->argv[1]) == -1){
            printf("Error: Cannot change directory to '%s'. %s.\n", params->argv[1], strerror(errno));
            //printf("argc: %i\n", params->argc);
        }
    } 
}

int main(int argc, char *argv[]) { 
    struct sigaction action;
    char buf[BUFSIZE];
    cmd_t params;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n" , strerror(errno));
        return EXIT_FAILURE;
    }

    uid_t uid = getuid();
    struct passwd *pw;
    if ((pw = getpwuid(uid)) == NULL) {
        fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    sigsetjmp(jmpbuf,1); 
    while(1) {
        char bf[BUFSIZE];
        if (getcwd(bf, sizeof(bf)) != NULL) {
            printf("[%s%s%s]$ ", BRIGHTBLUE, bf, DEFAULT);

        } else {
            fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
        }
        fflush(stdout);
        
        ssize_t bytes_read = read(STDIN_FILENO, buf, BUFSIZE - 1);
        if (bytes_read > 0) {
            buf[bytes_read - 1] = '\0';
        }
        if (bytes_read == 1) {
            continue;
        }
        //remove_space(input, buf);
        parse(buf, &params);

        if(strcmp(params.argv[0], "cd") == 0) {
            cd_func(&params);
        }
        else if(strcmp(params.argv[0], "exit") == 0) {
            break; 
        }
        else {
            execute(&params);
        }
    } 
    return EXIT_SUCCESS;
}
