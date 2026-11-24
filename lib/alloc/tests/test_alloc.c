#include "unit.h"

#include "alloc/alloc.h"
#include "alloc/arena.h"
#include "alloc/stack.h"

#include <stdint.h>
#include <string.h>

static bool test_stack_alignment_and_lifo(char *error, size_t error_size) {
    unsigned char storage[320];
    Alloc_Stack stack = alloc_stack(storage + 1, sizeof(storage) - 1);
    Alloc *allocator = ALLOC(&stack);

    unsigned char *first = alloc_new(allocator, 16);
    TEST_ASSERT(first != NULL, "first stack allocation must succeed");
    TEST_ASSERT((uintptr_t) first % _Alignof(max_align_t) == 0,
                "stack payload must be maximally aligned");
    memset(first, 0x5a, 16);

    unsigned char *second = alloc_new(allocator, 16);
    TEST_ASSERT(second != NULL, "second stack allocation must succeed");
    size_t used = stack.size;
    alloc_delete(allocator, first);
    TEST_ASSERT(stack.size == used, "deleting an older block must be a no-op");
    TEST_ASSERT(alloc_resize(allocator, second, 48) == second,
                "the active block must grow in place when it fits");
    alloc_delete(allocator, second);
    TEST_ASSERT(stack.size < used,
                "deleting the active block must reclaim space");
    TEST_ASSERT(alloc_resize(allocator, first, 80) == first,
                "the previous block must become active again");
    for (size_t index = 0; index < 16; index++) {
        TEST_ASSERT(first[index] == 0x5a, "in-place resize must preserve data");
    }

    alloc_finalize(allocator);
    TEST_ASSERT(stack.size == 0, "finalizing a stack must reset its position");
    return true;
}

static bool test_stack_move_and_failure(char *error, size_t error_size) {
    unsigned char storage[320];
    Alloc_Stack stack = alloc_stack(storage, sizeof(storage));
    Alloc *allocator = ALLOC(&stack);
    unsigned char *first = alloc_new(allocator, 8);
    unsigned char *second = alloc_new(allocator, 8);
    TEST_ASSERT(first != NULL && second != NULL, "small blocks must fit");
    memset(first, 0x3c, 8);

    unsigned char *replacement = alloc_resize(allocator, first, 32);
    TEST_ASSERT(replacement != NULL && replacement != first,
                "resizing an older block must move it");
    for (size_t index = 0; index < 8; index++) {
        TEST_ASSERT(replacement[index] == 0x3c, "moving must preserve data");
    }
    TEST_ASSERT(alloc_resize(allocator, replacement, SIZE_MAX) == NULL,
                "overflowing resize must fail");
    TEST_ASSERT(replacement[0] == 0x3c, "failure must preserve the old block");
    TEST_ASSERT(alloc_new(allocator, 1024) == NULL,
                "allocation beyond capacity must fail");
    TEST_ASSERT(alloc_resize(allocator, replacement, 0) == NULL,
                "zero resize must return null");
    alloc_reset(allocator);
    TEST_ASSERT(alloc_new(allocator, 8) != NULL,
                "reset must permit a new allocation");
    alloc_finalize(allocator);
    return true;
}

static bool test_legacy_stack_constructor(char *error, size_t error_size) {
    _Alignas(max_align_t) unsigned char storage[128];
    Alloc *allocator = alloc_stack_new(sizeof(storage), storage);
    TEST_ASSERT(allocator != NULL, "legacy constructor must succeed");
    TEST_ASSERT(new(allocator, 8) != NULL, "legacy allocation must succeed");
    finalize(allocator);
    return true;
}

static bool test_arena_segments_and_reset(char *error, size_t error_size) {
    Alloc *allocator = alloc_arena_new(80);
    TEST_ASSERT(allocator != NULL, "arena constructor must succeed");

    unsigned char *first = alloc_new(allocator, 32);
    TEST_ASSERT(first != NULL, "first arena allocation must succeed");
    TEST_ASSERT((uintptr_t) first % _Alignof(max_align_t) == 0,
                "arena payload must be maximally aligned");
    memset(first, 0xa5, 32);

    unsigned char *second = alloc_new(allocator, 32);
    TEST_ASSERT(second != NULL, "another segment must be available");
    TEST_ASSERT(alloc_resize(allocator, second, 16) == second,
                "active arena block must shrink in place");

    unsigned char *replacement = alloc_resize(allocator, first, 96);
    TEST_ASSERT(replacement != NULL && replacement != first,
                "older arena block must move to a larger segment");
    for (size_t index = 0; index < 32; index++) {
        TEST_ASSERT(replacement[index] == 0xa5, "moving must preserve data");
    }
    TEST_ASSERT(alloc_new(allocator, SIZE_MAX) == NULL,
                "overflowing arena request must fail");
    TEST_ASSERT(replacement[0] == 0xa5, "failure must preserve arena data");

    alloc_delete(allocator, second);
    alloc_reset(allocator);
    unsigned char *restarted = alloc_resize(allocator, NULL, 24);
    TEST_ASSERT(restarted != NULL, "reset arena must be reusable");
    TEST_ASSERT(alloc_resize(allocator, restarted, 0) == NULL,
                "zero resize must return null");
    TEST_ASSERT(alloc_new(allocator, 0) != NULL,
                "a zero-size request must get a slot");
    alloc_finalize(allocator);
    return true;
}

static bool test_null_dispatch(char *error, size_t error_size) {
    TEST_ASSERT(alloc_new(NULL, 16) == NULL,
                "null allocator must not allocate");
    TEST_ASSERT(alloc_resize(NULL, NULL, 16) == NULL,
                "null allocator must not resize");
    alloc_delete(NULL, NULL);
    alloc_reset(NULL);
    alloc_finalize(NULL);
    return true;
}

static const TestCase tests[] = {
    {"stack alignment and LIFO", test_stack_alignment_and_lifo},
    {"stack move and failure", test_stack_move_and_failure},
    {"legacy stack constructor", test_legacy_stack_constructor},
    {"arena segments and reset", test_arena_segments_and_reset},
    {"null dispatch", test_null_dispatch}
};

const TestSuite test_suite_alloc = {
    .name = "alloc",
    .tests = tests,
    .test_count = sizeof(tests) / sizeof(tests[0])
};
