#include "unit/unit.h"

extern const TestSuite test_suite_perceptron;

int main(void) {
    size_t passed = 0;
    size_t failed = 0;

    if (!test_run_suite(&test_suite_perceptron, &passed, &failed)) {
        failed++;
    }

    printf("\nTests: %zu, passed: %zu, failed: %zu\n",
            passed + failed, passed, failed);
    return failed == 0 ? 0 : 1;
}
