#include "alloc/pages.h"

#include <windows.h>

void *alloc_pages_new(size_t size) {
    if (size == 0) {
        return NULL;
    }
    return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void alloc_pages_delete(void *pointer, size_t size) {
    (void) size;
    if (pointer != NULL) {
        VirtualFree(pointer, 0, MEM_RELEASE);
    }
}
