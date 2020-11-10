/*******************************************************************************
 * Name        : quicksort.c
 * Author      : Brandon Cao    
 * Date        : Feb 15, 2020
 * Description : Quicksort implementation.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "quicksort.h"

/* Static (private to this file) function prototypes. */
static void swap(void *a, void *b, size_t size);
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*comp) (const void*, const void*));
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*comp) (const void*, const void*));

/**
 * Compares two integers passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to int pointers.
 * Returns:
 * -- 0 if the integers are equal
 * -- a positive number if the first integer is greater
 * -- a negative if the second integer is greater
 */
int int_cmp(const void *a, const void *b) {
    const int *num1 = (const int*)a;
    const int *num2 = (const int*)b;
    if(*num1 == *num2) {
        return 0;
    } else if (*num1 > *num2) {
        return 1;
    } else {
        return -1;
    }
}

/**
 * Compares two doubles passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to double pointers.
 * Returns:
 * -- 0 if the doubles are equal
 * -- 1 if the first double is greater
 * -- -1 if the second double is greater
 */
int dbl_cmp(const void *a, const void *b) {
    const double *num1 = (const double*)a;
    const double *num2 = (const double*)b;
    if(*num1 == *num2) {
        return 0;
    } else if (*num1 > *num2) {
        return 1;
    } else {
        return -1;
    }
}

/**
 * Compares two char arrays passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to char* pointers (i.e. char**).
 * Returns the result of calling strcmp on them.
 */
int str_cmp(const void *a, const void *b) { 
    const char **str1 = (const char **)a;
    const char **str2 = (const char **)b;
    return strcmp(*str1, *str2);
}

/**
 * Swaps the values in two pointers.
 *
 * Casts the void pointers to character types and works with them as char
 * pointers for the remainder of the function.
 * Swaps one byte at a time, until all 'size' bytes have been swapped.
 * For example, if ints are passed in, size will be 4. Therefore, this function
 * swaps 4 bytes in a and b character pointers.
 */

static void swap(void *a, void *b, size_t size) {
    char *num1 = (char *)a;
    char *num2 = (char *)b;
    char tmp;
    size_t i;

    for(i = 0; i < size; i++) {
        tmp = *(num1 + i);
        *(num1 + i) = *(num2 + i);
        *(num2 + i) = tmp;
    }
}

/**
 * Partitions array around a pivot, utilizing the swap function.
 * Each time the function runs, the pivot is placed into the correct index of
 * the array in sorted order. All elements less than the pivot should
 * be to its left, and all elements greater than or equal to the pivot should be
 * to its right.
 * The function pointer is dereferenced when it is used.
 * Indexing into void *array does not work. All work must be performed with
 * pointer arithmetic.
 */
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*comp) (const void*, const void*)) 
{
    char *p = (char*)((char *)array + left * elem_sz); 
    int s = left;

    for(unsigned int i = left + 1; i <= right; i++) {
        char *tmp = (char *)((char *)array + i * elem_sz);
        if(comp(p, tmp) >= 1) {
            s = s + 1;
            swap((char *)array + s * elem_sz, (char *)array + i * elem_sz, elem_sz);
        }
    }
    swap((char *)array + left * elem_sz, (char *)array + s * elem_sz, elem_sz);
    return s;
}

/**
 * Sorts with lomuto partitioning, with recursive calls on each side of the
 * pivot.
 * This is the function that does the work, since it takes in both left and
 * right index values.
 */
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*comp) (const void*, const void*)) {
    if(left < right) {
        int p = lomuto(array, left, right, elem_sz, comp);
        if(p > 0) {
            quicksort_helper(array, left, p-1, elem_sz, comp);
        }
        quicksort_helper(array, p+1, right, elem_sz, comp);
    }
    return;
}

/**
 * Quicksort function exposed to the user.
 * Calls quicksort_helper with left = 0 and right = len - 1.
 */
void quicksort(void *array, size_t len, size_t elem_sz,
               int (*comp) (const void*, const void*)) {
    quicksort_helper(array, 0, len-1, elem_sz, comp);
}
