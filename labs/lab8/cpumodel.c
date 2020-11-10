#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool starts_with(const char *str, const char *prefix) {
  /* TODO:
  Return true if the string starts with prefix, false otherwise.
  Note that prefix might be longer than the string itself.
  */
  if (strstr(str, prefix) != NULL) {
    return true;
  }
  return false;
}

int main() {
    /* TODO:
       Open "cat /proc/cpuinfo" for reading.
       If it fails, print the string "Error: popen() failed. %s.\n", where
       %s is strerror(errno) and return EXIT_FAILURE.
     */
    FILE *fp = popen("cat /proc/cpuinfo", "r");
    if (fp == NULL) {
      fprintf(stderr, "Error: popen() failed. %s.\n", strerror(errno));
      return EXIT_FAILURE;
    }


    /* TODO:
       Allocate an array of 256 characters on the stack.
       Use fgets to read line by line.
       If the line begins with "model name", print everything that comes after
       ": ".
       For example, with the line:
       model name      : AMD Ryzen 9 3900X 12-Core Processor
       print
       AMD Ryzen 9 3900X 12-Core Processor
       including the new line character.
       After you've printed it once, break the loop.
     */
    char arr[256];
    while(fgets(arr, sizeof(arr), fp) != NULL) {
      if(starts_with(arr, "model name")) {
          printf("%s\n", arr+13); 
          break;
      }
    }


    /* TODO:
       Close the file descriptor and check the status.
       If closing the descriptor fails, print the string
       "Error: pclose() failed. %s.\n", where %s is strerror(errno) and return
       EXIT_FAILURE.
     */
    int status = pclose(fp);
    if (status == -1) {
      fprintf(stderr, "Error: pclose() failed. %s.\n", strerror(errno));
      return EXIT_FAILURE;
    }

    return !(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}
