#ifndef ALLOC_PAGES_H
#define ALLOC_PAGES_H

#include <stddef.h>

/* Internal OS memory interface shared by the arena and platform sources. */
void *alloc_pages_new(size_t size);
void alloc_pages_delete(void *pointer, size_t size);

#endif
