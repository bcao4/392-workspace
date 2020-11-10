#include <errno.h>
#include <dirent.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "pfinds.h"

int main(int argc, char **argv) {
    char *directory = NULL;
    char *permission_string = NULL;
    int ret = 0; 
    char *path = NULL;

    ret = parse(argc, argv, &directory, &permission_string);
    if(ret == EXIT_FAILURE) {
        goto _End;
    }

    path = realpath(directory, NULL);
    /* printf("path = %s\n", path); */
    if(path && permission_string) {
        get_matching_permissions(path, permission_string);
    }

_End:
    if(directory){
        free(directory);
    }
    if(permission_string){
        free(permission_string);
    }
    if(path){
        free(path); 
    }
    return ret;
}

