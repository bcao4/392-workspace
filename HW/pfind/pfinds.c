#include <errno.h>
#include <dirent.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "pfinds.h"

static void usage() {
    printf("Usage: ./pfind -d <directory> -p <permission string> [-h]\n");
}

// Function to ensure that the permissions string is in proper format.
bool verify_permission_string(char *str) {
    if(str[0] != 'r' && str[0] != '-') {
        return false;
    }
    if(str[1] != 'w' && str[1] != '-') {
        return false;
    }
    if(str[2] != 'x' && str[2] != '-') {
        return false;
    }
    if(str[3] != 'r' && str[3] != '-') {
        return false;
    }
    if(str[4] != 'w' && str[4] != '-') {
        return false;
    }
    if(str[5] != 'x' && str[5] != '-') {
        return false;
    }
    if(str[6] != 'r' && str[6] != '-') {
        return false;
    }
    if(str[7] != 'w' && str[7] != '-') {
        return false;
    }
    if(str[8] != 'x' && str[8] != '-') {
        return false;
    }
    return true;
}

int parse(int argc, char **argv, char **directory, char **permission_string) {
    int c;

    if(argc == 1) {
        usage();
        return EXIT_FAILURE;
    }

    while(1) {
        int option_index = 0;
        static struct option long_options[] = 
        {
            {"-d",     required_argument, NULL,  0 },
            {"-p",     required_argument, NULL,  0 },
            {"-h",     no_argument,       NULL,  0 },
            {NULL,     0,                 NULL,  0 }
        };

    c = getopt_long(argc, argv, ":d:p:h", long_options, &option_index);
    if (c == -1) {
        break;
    }

    switch(c) {
        case 'd':
            *directory = strdup(optarg);
            break;

        case 'p':
            *permission_string = strdup(optarg);
            break;

        case 'h':
            usage();
            return EXIT_SUCCESS;

        case '?':
            printf("Error: Unknown option '-%c' received.\n", optopt);
            return EXIT_FAILURE;
    }
    }


    if(*directory == NULL) {
        printf("Error: Required argument -d <directory> not found.\n");
        return EXIT_FAILURE;
    }

    if(*permission_string == NULL) {
        printf("Error: Required argument -p <permissions string> not found.\n");
        return EXIT_FAILURE;
    } 

    if(*directory != NULL) {
        struct stat stats = {0};
        int ret = 0;

        ret = lstat(*directory, &stats);
        if (ret != 0) {   
            printf("Error: Cannot stat '%s'. %s\n", *directory, strerror(errno));
            return EXIT_FAILURE;
        }
    }
    
    if(*permission_string != NULL) {
        if(strlen(*permission_string) != 9) {
            printf("Error: Permission string '%s' is invalid.\n", *permission_string);
            return EXIT_FAILURE;
        }
        if(verify_permission_string(*permission_string) == false) {
            printf("Error: Permission string '%s' is invalid.\n", *permission_string);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

bool file_permission(char *permission_string, mode_t file_mode) {
    char permission[9] = {0};
    
    // Owner permissions
    if(file_mode & S_IRUSR) {
        permission[0] = 'r';
    } else {
        permission[0] = '-';
    }
    if(file_mode & S_IWUSR) {
        permission[1] = 'w';
    } else {
        permission[1] = '-';
    }
    if(file_mode & S_IXUSR) {
        permission[2] = 'x';
    } else {
        permission[2] = '-';
    }

    // Group permissions
    if(file_mode & S_IRGRP) {
        permission[3] = 'r';
    } else {
        permission[3] = '-';
    }
    if(file_mode & S_IWGRP) {
        permission[4] = 'w';
    } else {
        permission[4] = '-';
    }
    if(file_mode & S_IXGRP) {
        permission[5] = 'x';
    } else {
        permission[5] = '-';
    }


    // Other permissions
    if(file_mode & S_IROTH) {
        permission[6] = 'r';
    } else {
        permission[6] = '-';
    }
    if(file_mode & S_IWOTH) {
        permission[7] = 'w';
    } else {
        permission[7] = '-';
    }
    if(file_mode & S_IXOTH) {
        permission[8] = 'x';
    } else {
        permission[8] = '-';
    }

    if(memcmp(permission, permission_string, 9) == 0) {
        return true;
    } else {
        return false;
    }
}

int get_matching_permissions(char *directory, char *permission_string) {
    DIR *pDir = NULL;
    char *path = NULL;
    struct dirent *dp = NULL;
    struct stat fileInfo = {0};
    int len = 0, total_len = 0;;

    if(directory == NULL || permission_string == NULL)
        return -1;

    /* printf("dirname=%s, permit_string=%s\n", dirname, permit_string ); */
 
    len = strlen(directory);
    total_len = len + 128;
    path = malloc(total_len);
    if(path == NULL) {
        printf(" failed to malloc memory - %s\n", strerror(errno));
        return -1;
    } 
    memset(path, 0, total_len);

    if (!(pDir = opendir(directory))) {
        printf("Error: failed to opendir for %s. %s\n", path, strerror(errno));
        if(path) {
            free(path);
        }
        return EXIT_FAILURE;
    }
    while((dp = readdir(pDir))) {
        if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        memset(path, 0, total_len);
        snprintf(path, total_len, "%s/%s", directory, dp->d_name); 
        /* printf("Path: %s, len=%d\n", path, len); */
        if ((stat( path, &fileInfo)) < 0) {
            printf("Failed to stat path %s\n", path);
            continue;
        }
        if(fileInfo.st_mode & S_IFDIR) {
            /* printf("Dir: %s\n", path); */
            if(file_permission(permission_string, fileInfo.st_mode) == true) {
                printf("%s\n", directory);
            } else {
            get_matching_permissions(path, permission_string);
            }
        }
        else {
            if(file_permission(permission_string, fileInfo.st_mode) == false) {
                continue;
            } else {
            printf("%s\n", path);
            }
        }
    }    
    closedir(pDir);
    return 0;
}
