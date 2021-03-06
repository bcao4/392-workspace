/*******************************************************************************
 * Name        : sort.c
 * Author      : Brandon Cao
 * Date        : Feb 15, 2020
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"

#define MAX_STRLEN     64 // Not including '\0'
#define MAX_ELEMENTS 1024

typedef enum {
    STRING,
    INT,
    DOUBLE
} elem_t;

/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 */


static void usage() {
    printf("Usage: ./sort [-i|-d] [filename]\n");
    printf("   -i: Specifies the file contains ints.\n");
    printf("   -d: Specifies the file contains doubles.\n");
    printf("   filename: The file to sort.\n");
    printf("   No flags defaults to sorting strings.\n");
}


static void print_int_array(int array[], size_t sz) {
    for(int i = 0; i < sz; i++) {
        printf("%d\n", array[i]);
    }
    printf("\n");
}

static void print_double_array(double array[], size_t sz) {
    for(int i = 0; i < sz; i++) {
        printf("%f\n", array[i]);
    }
    printf("\n");
}


static void print_string_array(char *array[], size_t sz) {
    for(int i = 0; i < sz; i++) {
        printf("%s", array[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    int opt;
    int flag = 0; //String
    char *filename = NULL;

    while((opt = getopt(argc, argv, ":i:d:")) != -1) {
        switch(opt) {
            
            case 'i':
                flag = 1; //Ints
                filename = optarg;
                break;

            case 'd':
                flag = 2; //Doubles
                filename = optarg;
                break;

            case '?':
                printf("Error: Unknown option: '-%c' received.\n", optopt);

                break;

            case ':':
                printf("Missing arg for '-%c' received.\n", optopt);
                break;
        }
    }

    if(filename == NULL) {
        usage();
    }

    FILE *fptr;
    if(flag == 0) {
        filename = argv[1];
        if ((fptr = fopen(filename,"r")) == NULL){
            printf("Error: Cannot open '%s'. %s.\n", filename, strerror(errno));
            return EXIT_FAILURE;
        } 
    }

    if(flag == 1 || flag == 2) {
        if ((fptr = fopen(filename,"r")) == NULL){
            printf("Error: Cannot open '%s'. %s.\n", filename, strerror(errno));
            return EXIT_FAILURE;
        } 
    }

    size_t len = 0;
    char *line = NULL;
    int (*cmp[])(const void *a, const void *b) = {str_cmp, int_cmp, dbl_cmp}; 

    if(flag == 0) {
        char *str_array[MAX_ELEMENTS] = {0};
        int count = 0;
        int ret = 0;
        while ((ret = getline(&line, &len, fptr)) != -1) {
            line[ret-1] = '\n';
            str_array[count] = strdup(line);
            count++;
        }
        free(line);

        quicksort(str_array, count, sizeof(str_array[0]), cmp[flag]);  
        print_string_array(str_array, count);
        
        for(int i = 0; i <= count; i++) {
            if(str_array[i]) {
                free(str_array[i]);
            }
        }
   } 

    if(flag == 1) {
        int int_array[MAX_ELEMENTS] = {0};
        int count = 0;
        while(getline(&line, &len, fptr) != -1) {
            int_array[count] = atoi(line); 
            count++;
        }
        free(line);

        quicksort(int_array, count, sizeof(int_array[0]), cmp[flag]);  
        print_int_array( int_array, count );
   }

    if(flag == 2) {
        double double_array[MAX_ELEMENTS] = {0};
        int count = 0;
        while(getline(&line, &len, fptr) != -1) {
            double_array[count] = atoi(line); 
            count++;
        }
        free(line);

        quicksort(double_array, count, sizeof(double_array[0]), cmp[flag]);  
        print_double_array(double_array, count);
   }

   fclose(fptr);
   return 0;
}
