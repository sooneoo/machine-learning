#ifndef ALLOC_ALLOC_H
#define ALLOC_ALLOC_H

#include <stddef.h>

typedef struct Alloc Alloc;

typedef struct {
    void *(*new)(Alloc *self, size_t size);
    void (*delete)(Alloc *self, void *pointer);
    void *(*resize)(Alloc *self, void *pointer, size_t size);
    void (*reset)(Alloc *self);
    void (*finalize)(Alloc *self);
} Alloc_VTab;

struct Alloc {
    const Alloc_VTab *vtab;
};

#define ALLOC(value) ((Alloc *)(value))

static inline void *alloc_new(Alloc *self, size_t size) {
    return self != NULL && self->vtab != NULL && self->vtab->new != NULL
        ? self->vtab->new(self, size) : NULL;
}

static inline void alloc_delete(Alloc *self, void *pointer) {
    if (self != NULL && self->vtab != NULL && self->vtab->delete != NULL) {
        self->vtab->delete(self, pointer);
    }
}

static inline void *alloc_resize(Alloc *self, void *pointer, size_t size) {
    return self != NULL && self->vtab != NULL && self->vtab->resize != NULL
        ? self->vtab->resize(self, pointer, size) : NULL;
}

static inline void alloc_reset(Alloc *self) {
    if (self != NULL && self->vtab != NULL && self->vtab->reset != NULL) {
        self->vtab->reset(self);
    }
}

static inline void alloc_finalize(Alloc *self) {
    if (self != NULL && self->vtab != NULL && self->vtab->finalize != NULL) {
        self->vtab->finalize(self);
    }
}

/* Compatibility with the original API. Prefer the prefixed names in new code. */
static inline void *new(Alloc *self, size_t size) {
    return alloc_new(self, size);
}

static inline void delete(Alloc *self, void *pointer) {
    alloc_delete(self, pointer);
}

static inline void *resize(Alloc *self, void *pointer, size_t size) {
    return alloc_resize(self, pointer, size);
}

static inline void reset(Alloc *self) {
    alloc_reset(self);
}

static inline void finalize(Alloc *self) {
    alloc_finalize(self);
}

#endif
