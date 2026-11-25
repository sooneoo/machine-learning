#include "unit/unit.h"

static bool pass_case(char *error, size_t error_size) {
    (void) error;
    (void) error_size;
    return true;
}

static bool fail_case(char *error, size_t error_size) {
    TEST_ASSERT(false, "expected failure");
}

int main(void) {
    const TestCase cases[] = {
        {"pass", pass_case},
        {"fail", fail_case},
        {"missing callback", NULL}
    };
    const TestSuite suite = {"unit self-test", cases, 3};
    const TestSuite malformed = {"malformed", NULL, 1};
    size_t passed = 0;
    size_t failed = 0;

    if (!test_run_suite(&suite, &passed, &failed)) {
        return 1;
    }
    if (passed != 1 || failed != 2) {
        return 1;
    }
    if (test_run_suite(NULL, &passed, &failed)
            || test_run_suite(&suite, NULL, &failed)
            || test_run_suite(&suite, &passed, NULL)
            || test_run_suite(&malformed, &passed, &failed)) {
        return 1;
    }
    printf("\nHarness checks: passed\n");
    return 0;
}
