#ifndef ALLOC_STACK_H
#define ALLOC_STACK_H

#include "alloc/alloc.h"

typedef struct {
    Alloc super;
    size_t capacity;
    size_t size;
    unsigned char *front;
} Alloc_Stack;

#define ALLOC_STACK(value) ((Alloc_Stack *)(value))

/* The caller owns buffer and must keep it alive until the allocator is done. */
Alloc_Stack alloc_stack(void *buffer, size_t capacity);

/* Compatibility constructor: places Alloc_Stack at the start of buffer. */
Alloc *alloc_stack_new(size_t capacity, void *buffer);

#endif
