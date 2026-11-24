#ifndef ALLOC_ARENA_H
#define ALLOC_ARENA_H

#include "alloc/alloc.h"

/* The arena owns OS-backed segments and must be released with alloc_finalize. */
Alloc *alloc_arena_new(size_t segment_size);

#endif
