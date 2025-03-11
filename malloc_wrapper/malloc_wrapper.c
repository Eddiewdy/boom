#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>


void *malloc(size_t size) {
    static void *(*real_malloc)(size_t) = NULL;
    if (!real_malloc) {
        real_malloc = dlsym(RTLD_NEXT, "malloc");
        if (!real_malloc) {
            fprintf(stderr, "Error: dlsym failed to find malloc\n");
            exit(1);
        }
    }

    // 防止在 printf 内部调用 malloc 造成递归
    void *ptr = real_malloc(size);
    fprintf(stderr, "[Intercepted] malloc(%zu) = %p\n", size, ptr);
    return ptr;
}

void free(void *ptr) {
    static void (*real_free)(void *) = NULL;
    
    if (!real_free) {
        real_free = dlsym(RTLD_NEXT, "free");
        if (!real_free) {
            // fprintf(stderr, "Error in dlsym: %s\n", dlerror());
            return;
        }
    }

    printf("[Intercepted] free(%p)\n", ptr);
    real_free(ptr);
}
