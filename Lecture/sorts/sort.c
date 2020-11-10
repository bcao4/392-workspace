#include <stdio.h>
#include "sorts.h"

int main() {
    int nums[] = {4, -1, 3, 7, 2};
    const int len = sizeof(nums) / sizeof(int);
    printf("Array length: %d\n", len);
    display_array(nums, 5);
    insertion_sort(nums, 5);
    display_array(nums, 5);
    return 0;
}
