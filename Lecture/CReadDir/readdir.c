#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BUFSIZE 128

int main() {
    struct dirent *de;

    DIR *dr = opendir(".");
    if(dr == NULL) {
        printf("Error: Cannot opem current directory.\n");
        exit(EXIT_FAILURE);
    }

    char buf[PATH_MAX]; /* PATH_MAX includes the \0 so +1 is not required */
    //char timebuf[BUFSIZE];
    //struct stat b;

    while ((de = readdir(dr)) != NULL) {
        char *res = realpath(de->d_name, buf);
        if(!res) {

        }
        printf("%s\n", de->d_name);
    }

    closedir(dr);
    exit(EXIT_FAILURE);

}