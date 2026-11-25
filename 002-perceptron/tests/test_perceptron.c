#include "unit/unit.h"

#include "perceptron/optimizer.h"
#include "perceptron/perceptron.h"

#include <stdlib.h>

static bool close_float(float left, float right, float tolerance) {
    float difference = left - right;

    return difference > -tolerance && difference < tolerance;
}


static float reference_mse(Perceptron *model,
        const float inputs[][PERCEPTRON_INPUT_COUNT],
        const float targets[], size_t count) {
    float sum = 0.0f;

    for (size_t index = 0; index < count; index++) {
        float residual = perceptron_forward(model, inputs[index])
                - targets[index];

        sum += residual * residual;
    }

    return sum / (float) count;
}


static bool test_constructor(char *error, size_t error_size) {
    srand(42);
    Perceptron first = perceptron();
    Perceptron second = perceptron();
    srand(42);
    Perceptron repeated_first = perceptron();
    Perceptron repeated_second = perceptron();

    for (size_t index = 0; index < PERCEPTRON_INPUT_COUNT; index++) {
        TEST_ASSERT(first.w[index] >= -1.0f && first.w[index] <= 1.0f,
                "first weights must be within range");
        TEST_ASSERT(second.w[index] >= -1.0f && second.w[index] <= 1.0f,
                "second weights must be within range");
        TEST_ASSERT(first.w[index] == repeated_first.w[index],
                "first seeded weights must repeat");
        TEST_ASSERT(second.w[index] == repeated_second.w[index],
                "second seeded weights must repeat");
    }

    TEST_ASSERT(first.b >= -1.0f && first.b <= 1.0f,
            "first bias must be within range");
    TEST_ASSERT(second.b >= -1.0f && second.b <= 1.0f,
            "second bias must be within range");
    TEST_ASSERT(first.b == repeated_first.b && second.b == repeated_second.b,
            "seeded biases must repeat");
    return true;
}


static bool test_forward(char *error, size_t error_size) {
    Perceptron model = {0};
    const float zero[PERCEPTRON_INPUT_COUNT] = {0.0f, 0.0f};
    const float positive[PERCEPTRON_INPUT_COUNT] = {1.0f, 0.0f};
    const float negative[PERCEPTRON_INPUT_COUNT] = {-1.0f, 0.0f};

    TEST_ASSERT(perceptron_forward(&model, zero) == 0.5f,
            "zero logit must produce one half");
    model.w[0] = 1.0f;
    float high = perceptron_forward(&model, positive);
    float low = perceptron_forward(&model, negative);

    TEST_ASSERT(close_float(high, 0.7310586f, 0.000001f),
            "positive logit sigmoid");
    TEST_ASSERT(close_float(low, 0.2689414f, 0.000001f),
            "negative logit sigmoid");
    TEST_ASSERT(high > 0.5f && low < 0.5f,
            "sigmoid must be increasing");
    TEST_ASSERT(model.w[0] == 1.0f && model.b == 0.0f,
            "forward must not change model parameters");
    return true;
}


static bool test_optimizer_initialization(char *error, size_t error_size) {
    Perceptron model = {.w = {0.5f, -0.25f}, .b = 0.125f};
    Optimizer training = optimizer(&model);

    TEST_ASSERT(training.model == &model, "optimizer must borrow the model");
    TEST_ASSERT(training.backward == optimizer_backward_mse,
            "optimizer must select MSE backward by default");
    TEST_ASSERT(training.gw[0] == 0.0f && training.gw[1] == 0.0f,
            "weight gradients must start at zero");
    TEST_ASSERT(training.gb == 0.0f, "bias gradient must start at zero");
    optimizer_update(&training, 0.1f);
    TEST_ASSERT(model.w[0] == 0.5f && model.w[1] == -0.25f
            && model.b == 0.125f,
            "update before backward must leave the model unchanged");
    return true;
}


static bool test_batch_mse_and_bias_update(char *error, size_t error_size) {
    Perceptron model = {0};
    Optimizer training = optimizer(&model);
    const float inputs[][PERCEPTRON_INPUT_COUNT] = {
        {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}
    };
    const float targets[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float loss = optimizer_backward(&training, inputs, targets, 4);

    TEST_ASSERT(close_float(loss, 0.25f, 0.000001f),
            "zero model batch MSE");
    TEST_ASSERT(close_float(training.gw[0], 0.0f, 0.000001f)
            && close_float(training.gw[1], 0.0f, 0.000001f),
            "symmetric zero model has zero weight gradients");
    TEST_ASSERT(close_float(training.gb, 0.125f, 0.000001f),
            "zero model bias gradient");
    TEST_ASSERT(model.w[0] == 0.0f && model.w[1] == 0.0f
            && model.b == 0.0f,
            "backward must not change the model");
    optimizer_update(&training, 0.1f);
    TEST_ASSERT(close_float(model.b, -0.0125f, 0.000001f),
            "bias update after batch backward");
    return true;
}


static bool test_single_row_weight_update(char *error, size_t error_size) {
    Perceptron model = {0};
    Optimizer training = optimizer(&model);
    const float inputs[][PERCEPTRON_INPUT_COUNT] = {{1.0f, 1.0f}};
    const float targets[] = {1.0f};
    float loss = optimizer_backward(&training, inputs, targets, 1);

    TEST_ASSERT(close_float(loss, 0.25f, 0.000001f),
            "single row squared loss");
    TEST_ASSERT(close_float(training.gw[0], -0.25f, 0.000001f)
            && close_float(training.gw[1], -0.25f, 0.000001f),
            "single row weight gradients");
    TEST_ASSERT(close_float(training.gb, -0.25f, 0.000001f),
            "single row bias gradient");
    optimizer_update(&training, 0.1f);
    TEST_ASSERT(close_float(model.w[0], 0.025f, 0.000001f)
            && close_float(model.w[1], 0.025f, 0.000001f)
            && close_float(model.b, 0.025f, 0.000001f),
            "single row parameter updates");
    return true;
}


static bool test_gradients_against_finite_difference(
        char *error, size_t error_size) {
    Perceptron model = {.w = {0.4f, -0.3f}, .b = 0.2f};
    Optimizer training = optimizer(&model);
    const float inputs[][PERCEPTRON_INPUT_COUNT] = {
        {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}
    };
    const float targets[] = {0.0f, 0.0f, 0.0f, 1.0f};
    const float epsilon = 0.001f;
    float *parameters[] = {&model.w[0], &model.w[1], &model.b};
    float gradients[3];

    optimizer_backward(&training, inputs, targets, 4);
    gradients[0] = training.gw[0];
    gradients[1] = training.gw[1];
    gradients[2] = training.gb;

    for (size_t index = 0; index < 3; index++) {
        float original = *parameters[index];

        *parameters[index] = original + epsilon;
        float plus = reference_mse(&model, inputs, targets, 4);
        *parameters[index] = original - epsilon;
        float minus = reference_mse(&model, inputs, targets, 4);
        *parameters[index] = original;
        float numerical = (plus - minus) / (2.0f * epsilon);

        TEST_ASSERT(close_float(gradients[index], numerical, 0.0001f),
                "analytic gradient must match finite difference");
    }

    return true;
}


static float replacement_backward(Optimizer *self,
        const float inputs[][PERCEPTRON_INPUT_COUNT],
        const float targets[], size_t count) {
    self->gw[0] = inputs[0][0];
    self->gw[1] = inputs[0][1];
    self->gb = targets[0] + (float) count;
    return 7.0f;
}


static bool test_backward_callback(char *error, size_t error_size) {
    Perceptron model = {0};
    Optimizer training = optimizer(&model);
    const float inputs[][PERCEPTRON_INPUT_COUNT] = {{1.0f, 0.0f}};
    const float targets[] = {1.0f};

    training.backward = replacement_backward;
    float loss = optimizer_backward(&training, inputs, targets, 1);

    TEST_ASSERT(loss == 7.0f, "wrapper must return selected callback loss");
    TEST_ASSERT(training.gw[0] == 1.0f && training.gw[1] == 0.0f
            && training.gb == 2.0f,
            "wrapper must pass batch and optimizer to callback");
    return true;
}


static const TestCase tests[] = {
    {"seeded constructor", test_constructor},
    {"sigmoid forward", test_forward},
    {"optimizer initialization", test_optimizer_initialization},
    {"batch MSE and bias update", test_batch_mse_and_bias_update},
    {"single row weight update", test_single_row_weight_update},
    {"analytic gradients", test_gradients_against_finite_difference},
    {"selectable backward callback", test_backward_callback}
};


const TestSuite test_suite_perceptron = {
    .name = "sigmoid perceptron",
    .tests = tests,
    .test_count = sizeof(tests) / sizeof(tests[0])
};
