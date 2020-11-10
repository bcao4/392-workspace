#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    void *handle;
    void (*my_fn)();

    if ((handle = dlopen("./my_dl.so", RTLD_LAZY)) == NULL) {
        fprintf(stderr, "Error: %s.\n", dlerror());
        return EXIT_FAILURE;
    }

    *(void**)&my_fn = dlsym(handle, "my_fn");
    if (my_fn == NULL) {
        fprintf(stderr, "Error: %s.\n", dlerror());
        return EXIT_FAILURE;
    }

    printf("------ C program: Calling my_fn() the first time ------\n");
    my_fn();
    printf("-------------------------------------------------------\n");

    if (dlclose(handle) < 0) {
        fprintf(stderr, "Error: %s.\n", dlerror());
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 5; i++) {
        sleep(1);
        printf("C program: Sleep #%d.\n", i);
    }

    if ((handle = dlopen("./my_dl.so", RTLD_LAZY)) == NULL) {
        fprintf(stderr, "Error: %s.\n", dlerror());
        return EXIT_FAILURE;
    }

    *(void**)&my_fn = dlsym(handle, "my_fn");
    if (my_fn == NULL) {
        fprintf(stderr, "Error: %s.\n", dlerror());
        return EXIT_FAILURE;
    }

    printf("------ C program: Calling my_fn() the second time -----\n");
    my_fn();
    printf("-------------------------------------------------------\n");

    if (dlclose(handle) < 0) {
        fprintf(stderr, "Error: %s.\n", dlerror());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
