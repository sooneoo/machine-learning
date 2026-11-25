#ifndef UNIT_UNIT_H
#define UNIT_UNIT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef bool (*TestFunction)(char *error, size_t error_size);

typedef struct {
    const char *name;
    TestFunction function;
} TestCase;

typedef struct {
    const char *name;
    const TestCase *tests;
    size_t test_count;
} TestSuite;

#define TEST_ASSERT(condition, message) do { \
    if (!(condition)) { \
        snprintf(error, error_size, "%s", (message)); \
        return false; \
    } \
} while (0)

#define TEST_ASSERT_MSG(condition, format, ...) do { \
    if (!(condition)) { \
        snprintf(error, error_size, (format), __VA_ARGS__); \
        return false; \
    } \
} while (0)

bool test_run_suite(const TestSuite *suite, size_t *passed, size_t *failed);

#endif
