#include "alloc/arena.h"
#include "alloc/pages.h"

#include <stdint.h>
#include <string.h>

typedef struct ArenaSegment ArenaSegment;

struct ArenaSegment {
    /* The newest segment is the only one eligible for a LIFO pop. */
    ArenaSegment *next;
    size_t region_size;
    size_t capacity;
    size_t used;
    unsigned char *data;
};

typedef struct {
    size_t previous;
    size_t requested;
} ArenaBlock;

typedef struct {
    Alloc super;
    size_t segment_size;
    ArenaSegment *head;
} Alloc_Arena;

static int align_up(size_t value, size_t alignment, size_t *result) {
    size_t remainder = value % alignment;
    size_t padding = remainder == 0 ? 0 : alignment - remainder;
    if (value > SIZE_MAX - padding) {
        return 0;
    }
    *result = value + padding;
    return 1;
}

static size_t block_header_size(void) {
    size_t result = 0;
    align_up(sizeof(ArenaBlock), _Alignof(max_align_t), &result);
    return result;
}

static ArenaSegment *segment_new(size_t capacity) {
    size_t prefix;
    if (!align_up(sizeof(ArenaSegment), _Alignof(max_align_t), &prefix)
            || capacity > SIZE_MAX - prefix) {
        return NULL;
    }

    size_t region_size = prefix + capacity;
    ArenaSegment *segment = alloc_pages_new(region_size);
    if (segment == NULL) {
        return NULL;
    }

    *segment = (ArenaSegment) {
        .next = NULL,
        .region_size = region_size,
        .capacity = capacity,
        .used = 0,
        .data = (unsigned char *) segment + prefix
    };
    return segment;
}

static ArenaBlock *find_block(
        Alloc_Arena *self,
        void *pointer,
        ArenaSegment **owner,
        size_t *start,
        size_t *end
) {
    if (pointer == NULL) {
        return NULL;
    }

    uintptr_t address = (uintptr_t) pointer;
    size_t header_size = block_header_size();
    for (ArenaSegment *segment = self->head;
            segment != NULL; segment = segment->next) {
        uintptr_t base = (uintptr_t) segment->data;
        if (address < base || address - base < header_size
                || address - base > segment->used) {
            continue;
        }

        size_t block_start = (size_t) (address - base) - header_size;
        if (block_start % _Alignof(max_align_t) != 0
                || block_start > segment->used) {
            continue;
        }

        ArenaBlock *block = (ArenaBlock *) (segment->data + block_start);
        size_t payload_size;
        size_t requested = block->requested == 0 ? 1 : block->requested;
        if (block->previous != block_start
                || !align_up(requested, _Alignof(max_align_t), &payload_size)
                || header_size > segment->used - block_start
                || payload_size > segment->used - block_start - header_size) {
            continue;
        }

        *owner = segment;
        *start = block_start;
        *end = block_start + header_size + payload_size;
        return block;
    }
    return NULL;
}

static void *arena_new(Alloc *allocator, size_t size) {
    Alloc_Arena *self = (Alloc_Arena *) allocator;
    size_t payload_size;
    size_t header_size = block_header_size();
    size_t requested = size == 0 ? 1 : size;
    if (!align_up(requested, _Alignof(max_align_t), &payload_size)
            || payload_size > SIZE_MAX - header_size) {
        return NULL;
    }

    size_t required = header_size + payload_size;
    ArenaSegment *segment = self->head;
    if (segment == NULL || segment->used > segment->capacity
            || required > segment->capacity - segment->used) {
        /* Large requests get a segment without changing the base size. */
        size_t capacity = required > self->segment_size
                ? required : self->segment_size;
        segment = segment_new(capacity);
        if (segment == NULL) {
            return NULL;
        }
        segment->next = self->head;
        self->head = segment;
    }

    ArenaBlock *block = (ArenaBlock *) (segment->data + segment->used);
    block->previous = segment->used;
    block->requested = size;
    segment->used += required;
    return (unsigned char *) block + header_size;
}

static void arena_delete(Alloc *allocator, void *pointer) {
    Alloc_Arena *self = (Alloc_Arena *) allocator;
    ArenaSegment *owner;
    size_t start;
    size_t end;
    ArenaBlock *block = find_block(self, pointer, &owner, &start, &end);
    if (block != NULL && owner == self->head && end == owner->used) {
        owner->used = block->previous;
    }
}

static void *arena_resize(Alloc *allocator, void *pointer, size_t size) {
    Alloc_Arena *self = (Alloc_Arena *) allocator;
    if (pointer == NULL) {
        return arena_new(allocator, size);
    }
    if (size == 0) {
        arena_delete(allocator, pointer);
        return NULL;
    }

    ArenaSegment *owner;
    size_t start;
    size_t end;
    ArenaBlock *block = find_block(self, pointer, &owner, &start, &end);
    if (block == NULL) {
        return NULL;
    }

    size_t payload_size;
    size_t header_size = block_header_size();
    if (!align_up(size, _Alignof(max_align_t), &payload_size)) {
        return NULL;
    }

    if (owner == self->head && end == owner->used
            && header_size <= owner->capacity - start
            && payload_size <= owner->capacity - start - header_size) {
        owner->used = start + header_size + payload_size;
        block->requested = size;
        return pointer;
    }

    size_t copy_size = block->requested < size ? block->requested : size;
    void *replacement = arena_new(allocator, size);
    if (replacement != NULL) {
        memcpy(replacement, pointer, copy_size);
    }
    return replacement;
}

static void arena_reset(Alloc *allocator) {
    Alloc_Arena *self = (Alloc_Arena *) allocator;
    ArenaSegment *segment = self->head;
    self->head = NULL;
    while (segment != NULL) {
        ArenaSegment *next = segment->next;
        size_t region_size = segment->region_size;
        alloc_pages_delete(segment, region_size);
        segment = next;
    }
}

static void arena_finalize(Alloc *allocator) {
    arena_reset(allocator);
    alloc_pages_delete(allocator, sizeof(Alloc_Arena));
}

static const Alloc_VTab arena_vtab = {
    .new = arena_new,
    .delete = arena_delete,
    .resize = arena_resize,
    .reset = arena_reset,
    .finalize = arena_finalize
};

Alloc *alloc_arena_new(size_t segment_size) {
    Alloc_Arena *self = alloc_pages_new(sizeof(Alloc_Arena));
    if (self == NULL) {
        return NULL;
    }

    *self = (Alloc_Arena) {
        .super = {.vtab = &arena_vtab},
        .segment_size = segment_size == 0 ? 4096 : segment_size,
        .head = NULL
    };
    return ALLOC(self);
}
