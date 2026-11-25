#include "unit/unit.h"

bool test_run_suite(const TestSuite *suite, size_t *passed, size_t *failed) {
    if (suite == NULL || passed == NULL || failed == NULL
            || (suite->tests == NULL && suite->test_count != 0)) {
        return false;
    }

    printf("\n[%s]\n", suite->name != NULL ? suite->name : "unnamed");
    for (size_t index = 0; index < suite->test_count; index++) {
        const TestCase *test = &suite->tests[index];
        const char *name = test->name != NULL ? test->name : "unnamed";
        char error[256] = {0};
        bool ok = test->function != NULL
                && test->function(error, sizeof(error));

        if (ok) {
            printf("  [PASS] %s\n", name);
            (*passed)++;
        } else {
            const char *reason = error[0] != '\0'
                    ? error : "unspecified failure";
            printf("  [FAIL] %s: %s\n", name, reason);
            (*failed)++;
        }
    }
    return true;
}
