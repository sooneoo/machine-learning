#include "alloc/stack.h"

#include <stdint.h>
#include <string.h>

typedef struct {
    /* Offsets are relative to the aligned caller-owned buffer. */
    size_t previous;
    size_t requested;
} StackBlock;

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
    align_up(sizeof(StackBlock), _Alignof(max_align_t), &result);
    return result;
}

static StackBlock *find_block(
        Alloc_Stack *self, void *pointer, size_t *start, size_t *end
) {
    if (pointer == NULL || self->front == NULL) {
        return NULL;
    }

    uintptr_t address = (uintptr_t) pointer;
    uintptr_t base = (uintptr_t) self->front;
    size_t header_size = block_header_size();
    if (address < base || address - base < header_size
            || address - base > self->size) {
        return NULL;
    }

    size_t block_start = (size_t) (address - base) - header_size;
    if (block_start % _Alignof(max_align_t) != 0 || block_start > self->size) {
        return NULL;
    }

    StackBlock *block = (StackBlock *) (self->front + block_start);
    size_t payload_size;
    size_t requested = block->requested == 0 ? 1 : block->requested;
    if (block->previous != block_start
            || !align_up(requested, _Alignof(max_align_t), &payload_size)
            || header_size > self->size - block_start
            || payload_size > self->size - block_start - header_size) {
        return NULL;
    }

    *start = block_start;
    *end = block_start + header_size + payload_size;
    return block;
}

static void *stack_new(Alloc *allocator, size_t size) {
    Alloc_Stack *self = (Alloc_Stack *) allocator;
    size_t header_size = block_header_size();
    size_t payload_size;
    size_t requested = size == 0 ? 1 : size;
    if (self->front == NULL || self->size > self->capacity
            || !align_up(requested, _Alignof(max_align_t), &payload_size)
            || header_size > self->capacity - self->size
            || payload_size > self->capacity - self->size - header_size) {
        return NULL;
    }

    StackBlock *block = (StackBlock *) (self->front + self->size);
    block->previous = self->size;
    block->requested = size;
    self->size += header_size + payload_size;
    return (unsigned char *) block + header_size;
}

static void stack_delete(Alloc *allocator, void *pointer) {
    Alloc_Stack *self = (Alloc_Stack *) allocator;
    size_t start;
    size_t end;
    StackBlock *block = find_block(self, pointer, &start, &end);
    /* An older block stays live until every newer block is removed. */
    if (block != NULL && end == self->size) {
        self->size = block->previous;
    }
}

static void *stack_resize(Alloc *allocator, void *pointer, size_t size) {
    Alloc_Stack *self = (Alloc_Stack *) allocator;
    if (pointer == NULL) {
        return stack_new(allocator, size);
    }
    if (size == 0) {
        stack_delete(allocator, pointer);
        return NULL;
    }

    size_t start;
    size_t end;
    StackBlock *block = find_block(self, pointer, &start, &end);
    if (block == NULL) {
        return NULL;
    }

    size_t payload_size;
    size_t header_size = block_header_size();
    if (!align_up(size, _Alignof(max_align_t), &payload_size)) {
        return NULL;
    }

    if (end == self->size && header_size <= self->capacity - start
            && payload_size <= self->capacity - start - header_size) {
        self->size = start + header_size + payload_size;
        block->requested = size;
        return pointer;
    }

    /* Moving an older block leaves its old slot for a later reset. */
    size_t copy_size = block->requested < size ? block->requested : size;
    void *replacement = stack_new(allocator, size);
    if (replacement != NULL) {
        memcpy(replacement, pointer, copy_size);
    }
    return replacement;
}

static void stack_reset(Alloc *allocator) {
    ((Alloc_Stack *) allocator)->size = 0;
}

static const Alloc_VTab stack_vtab = {
    .new = stack_new,
    .delete = stack_delete,
    .resize = stack_resize,
    .reset = stack_reset,
    .finalize = stack_reset
};

Alloc_Stack alloc_stack(void *buffer, size_t capacity) {
    Alloc_Stack result = {
        .super = {.vtab = &stack_vtab},
        .capacity = 0,
        .size = 0,
        .front = NULL
    };
    if (buffer == NULL) {
        return result;
    }

    uintptr_t address = (uintptr_t) buffer;
    size_t alignment = _Alignof(max_align_t);
    size_t padding = (alignment - address % alignment) % alignment;
    if (padding <= capacity) {
        result.front = (unsigned char *) buffer + padding;
        result.capacity = capacity - padding;
    }
    return result;
}

Alloc *alloc_stack_new(size_t capacity, void *buffer) {
    if (buffer == NULL || capacity < sizeof(Alloc_Stack)
            || (uintptr_t) buffer % _Alignof(Alloc_Stack) != 0) {
        return NULL;
    }

    Alloc_Stack *self = (Alloc_Stack *) buffer;
    *self = alloc_stack(
            (unsigned char *) buffer + sizeof(*self), capacity - sizeof(*self)
    );
    return ALLOC(self);
}
