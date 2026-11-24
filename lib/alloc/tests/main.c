#include "unit.h"

extern const TestSuite test_suite_alloc;

int main(void) {
    const TestSuite *suites[] = {&test_suite_alloc};
    size_t passed = 0;
    size_t failed = 0;

    size_t suite_count = sizeof(suites) / sizeof(suites[0]);
    for (size_t index = 0; index < suite_count; index++) {
        if (!test_run_suite(suites[index], &passed, &failed)) {
            failed++;
        }
    }

    printf("\nTests: %zu, passed: %zu, failed: %zu\n",
            passed + failed, passed, failed);
    return failed == 0 ? 0 : 1;
}
