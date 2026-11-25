#include "unit/unit.h"

#include "regression/optimizer.h"
#include "regression/regression.h"

#include <stdlib.h>

static bool close_float(float left, float right) {
    float difference = left - right;
    return difference > -0.000001f && difference < 0.000001f;
}

static bool test_forward(char *error, size_t error_size) {
    Regression model = {.a = 2.0f, .b = 1.0f};
    TEST_ASSERT(close_float(regression_forward(&model, 3.0f), 7.0f),
            "affine forward result");
    model.b = 0.0f;
    TEST_ASSERT(close_float(regression_forward(&model, 3.0f), 6.0f),
            "zero intercept result");
    model = (Regression) {.a = -1.0f, .b = 2.0f};
    TEST_ASSERT(close_float(regression_forward(&model, -2.0f), 4.0f),
            "negative input result");
    TEST_ASSERT(model.a == -1.0f && model.b == 2.0f,
            "forward must not change parameters");
    return true;
}

static bool test_constructor(char *error, size_t error_size) {
    srand(42);
    Regression first = regression();
    Regression second = regression();
    srand(42);
    Regression repeated_first = regression();
    Regression repeated_second = regression();
    TEST_ASSERT(first.a >= -1.0f && first.a <= 1.0f,
            "first slope must be within range");
    TEST_ASSERT(first.b >= -1.0f && first.b <= 1.0f,
            "first intercept must be within range");
    TEST_ASSERT(second.a >= -1.0f && second.a <= 1.0f,
            "second slope must be within range");
    TEST_ASSERT(second.b >= -1.0f && second.b <= 1.0f,
            "second intercept must be within range");
    TEST_ASSERT(first.a == repeated_first.a && first.b == repeated_first.b,
            "fixed seed must reproduce the first model");
    TEST_ASSERT(second.a == repeated_second.a
            && second.b == repeated_second.b,
            "fixed seed must reproduce the second model");
    return true;
}

static bool test_optimizer_initialization(char *error, size_t error_size) {
    Regression model = {.a = 0.5f, .b = -0.25f};
    Optimizer training = optimizer(&model);
    TEST_ASSERT(training.regression == &model, "optimizer must retain model");
    TEST_ASSERT(training.ga == 0.0f && training.gb == 0.0f,
            "initial gradients must be zero");
    optimizer_update(&training, 0.1f);
    TEST_ASSERT(model.a == 0.5f && model.b == -0.25f,
            "update before backward must leave model unchanged");
    return true;
}

static bool test_backward_and_update(char *error, size_t error_size) {
    Regression model = {0};
    Optimizer training = optimizer(&model);
    float loss = optimizer_backward(&training, 2.0f, 1.0f);
    TEST_ASSERT(close_float(loss, 1.0f), "squared loss for one sample");
    TEST_ASSERT(close_float(training.ga, -4.0f), "slope gradient");
    TEST_ASSERT(close_float(training.gb, -2.0f), "intercept gradient");
    TEST_ASSERT(model.a == 0.0f && model.b == 0.0f,
            "backward must leave model unchanged");
    optimizer_update(&training, 0.1f);
    TEST_ASSERT(close_float(model.a, 0.4f), "updated slope");
    TEST_ASSERT(close_float(model.b, 0.2f), "updated intercept");
    return true;
}

static bool test_squared_loss_and_reference(char *error, size_t error_size) {
    Regression model = {0};
    Optimizer training = optimizer(&model);
    float first = optimizer_backward(&training, 1.0f, 1.0f);
    float second = optimizer_backward(&training, 3.0f, 2.0f);
    TEST_ASSERT(close_float((first + second) / 2.0f, 2.5f),
            "mean squared loss of two points");
    model = (Regression) {.a = 0.625f, .b = 0.0f};
    const float x[] = {1.0f, 3.0f, 5.0f};
    const float y[] = {0.625f, 1.875f, 3.125f};
    for (size_t index = 0; index < 3; index++) {
        TEST_ASSERT(optimizer_backward(&training, x[index], y[index]) == 0.0f,
                "reference line must fit each training point");
    }
    return true;
}

static const TestCase tests[] = {
    {"forward", test_forward},
    {"seeded constructor", test_constructor},
    {"optimizer initialization", test_optimizer_initialization},
    {"backward and update", test_backward_and_update},
    {"squared loss and reference", test_squared_loss_and_reference}
};

const TestSuite test_suite_regression = {
    .name = "linear regression",
    .tests = tests,
    .test_count = sizeof(tests) / sizeof(tests[0])
};
