/*******************************************************************************
# * Name        : chatclient.c
# * Author      : Brandon Cao 
# * Date        : 4/28/20
# * Description : Chat client
# * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
# ******************************************************************************/
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "util.h"

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];
bool gbStopRunning = false;

int handle_client_socket(int socket_fd) {
    int bytes_recvd = recv(socket_fd, inbuf, MAX_MSG_LEN, 0);
    if (bytes_recvd == -1) {
        fprintf(stderr, "Warning: Failed to receive user name. %s.\n", strerror(errno));
    } 
    else if(strncasecmp(inbuf, "BYE", 3) == 0) {
        printf("\nServer initiated shutdown.\n");
        gbStopRunning = true;
    }
    else if (bytes_recvd == 0) {
        /* The server abruptly broke the connection with client */
        printf("\nConnection to server has been lost.\n");
        gbStopRunning = true;
    } else {
        inbuf[bytes_recvd] = '\0';
        printf("\n%s\n", inbuf);
        printf("[%s]: ", username);
    }
    fflush(stdout);
    return EXIT_SUCCESS;
}

int handle_stdin(int socket_fd) {
    int ret = 0;
    ret = get_string(outbuf, MAX_MSG_LEN);
    
    if(ret == NO_INPUT) {
        ret = EXIT_SUCCESS;
    }

    else if(ret == TOO_LONG) {
        fprintf(stderr, "Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
        ret = EXIT_SUCCESS;
    }

    else if(strncasecmp(outbuf, "BYE", 3) == 0) {
        if(send(socket_fd, outbuf, strlen(outbuf), 0) == -1) {
            fprintf(stderr, "Warning: Failed to send message. %s.\n", strerror(errno));
        }
        gbStopRunning = true;
        return EXIT_SUCCESS;
    }

    else if(send(socket_fd, outbuf, strlen(outbuf), 0) == -1) {
        fprintf(stderr, "Warning: Failed to send message. %s.\n", strerror(errno));
    }

    printf("[%s]: ", username);
    fflush(stdout);
    memset(outbuf, 0, MAX_MSG_LEN);
    return ret;
}

void catch_signal(int sig) {
    gbStopRunning = true;
}

int main(int argc, char *argv[]) {
    int ret = 0;
    int port;   
    struct sockaddr_in  server;
    int socket_fd = -1;
    // void (*signalHandler)(int);

    // Set up a signal handler for SIGINT, CTRL+C.
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;
    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "Error: Failed to register signal handler. %s.\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

/* Parse command line argument for IP address and port number. */
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <server IP> <port number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    if(!parse_int(argv[2], &port, "port number")) {
        return EXIT_FAILURE;
    }
    if(port < 1024 || port > 65535) {
        fprintf(stderr, "Error: port must be in range [1024, 65535].\n");
        return EXIT_FAILURE;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    ret = inet_pton(AF_INET, argv[1], &server.sin_addr);
    if(ret == 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'.\n", argv[1]);
        return EXIT_FAILURE;
    }

/* Prompting for a Username */
    fprintf(stdout, "Enter your username: ");
    if(fgets(inbuf, BUFLEN, stdin ) == 0) {
        fprintf(stdout, "Error: %s. \n", strerror(errno)); 
        return EXIT_FAILURE;
    }
    if(strlen(inbuf) > MAX_NAME_LEN + 1) {
        fprintf(stderr, "Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
        return EXIT_FAILURE;
    }
    else {
        inbuf[strlen(inbuf) - 1] = '\0';
        memcpy(username, inbuf, MAX_NAME_LEN);
    }
    printf("Hello, %s. Let's try to connect to the server.\n", username);

/* Establishing Connection */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if(socket_fd == -1) { 
        fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno)); 
        return EXIT_FAILURE;
    } 
    else {
        printf("Socket successfully created..\n"); 
    }
    if(connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) != 0) { 
        fprintf(stderr, "Error: Failed to connect to server. %s.\n", strerror(errno)); 
        ret = EXIT_FAILURE;
        goto _End;
    } 
    else {
        printf("connected to the server..\n"); 
    }
    ret = read(socket_fd, inbuf, sizeof(inbuf));
    if(ret == -1) {
        fprintf(stderr, "Error: Failed to receive message from server. %s.\n", strerror(errno));
        ret = EXIT_FAILURE;
        goto _End;
    }
    printf("\n%s\n", inbuf);
    write(socket_fd, username, MAX_NAME_LEN);
    printf("[%s]: ", username);
    fflush(stdout);

/* Handling Activity on File Descriptors (Sockets) */
    fd_set sockset;
    int max_socket;
    while(1) {
        if(gbStopRunning) {
            break;
        }
        FD_ZERO(&sockset);
        max_socket = STDIN_FILENO;

        if(socket_fd > STDIN_FILENO) {
            max_socket = socket_fd; 
        }
        max_socket += 1;
       
        FD_SET(STDIN_FILENO, &sockset);
        FD_SET(socket_fd, &sockset);

        if(select(max_socket, &sockset, NULL, NULL, NULL) < 0 && errno != EINTR) {
            fprintf(stderr, "Error: select() failed. %s.\n", strerror(errno));
            ret = EXIT_FAILURE;
            break;
        }

        if(gbStopRunning) {
            printf("\n");
            break;
        }
        else if(FD_ISSET(socket_fd, &sockset)) {
            if(handle_client_socket( socket_fd ) == EXIT_FAILURE) {
                ret = EXIT_FAILURE;
                break;
            }
        }
        else if(FD_ISSET(STDIN_FILENO, &sockset)) {
            handle_stdin( socket_fd ); 
        }
    }

_End:
    if(socket_fd > 0) {
        close(socket_fd);
    }
    return ret;
}
