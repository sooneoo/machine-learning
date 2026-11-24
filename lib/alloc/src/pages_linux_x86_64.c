#include "alloc/pages.h"

#include <sys/mman.h>

void *alloc_pages_new(size_t size) {
    if (size == 0) {
        return NULL;
    }

    void *result = mmap(NULL, size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return result == MAP_FAILED ? NULL : result;
}

void alloc_pages_delete(void *pointer, size_t size) {
    if (pointer != NULL && size != 0) {
        munmap(pointer, size);
    }
}
