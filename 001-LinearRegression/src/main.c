/**
 * @file main.c
 * @brief Run SGD and BGD experiments and evaluate held-out points.
 */

#include "regression/optimizer.h"
#include "regression/regression.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Train with one update per sample and validate the final model.
 * @param[in] lr Learning rate for all ten epochs.
 * @note Validation samples are not passed to the optimizer.
 */
static void run_sgd_experiment(float lr) {
    srand(42);
    Regression model = regression();
    Optimizer training = optimizer(&model);
    const float x[] = {1.0f, 3.0f, 5.0f};
    const float y[] = {0.625f, 1.875f, 3.125f};
    const float validation_x[] = {2.0f, 4.0f, 6.0f};
    const float validation_y[] = {1.25f, 2.5f, 3.75f};
    size_t epoch = 10;
    size_t point_count = sizeof(x) / sizeof(x[0]);

    printf("SGD: seed=42 lr=%.9g epochs=%zu initial_a=%.9g"
            " initial_b=%.9g\n", lr, epoch, model.a, model.b);

    for (size_t epoch_index = 0; epoch_index < epoch; epoch_index++) {
        for (size_t point_index = 0; point_index < point_count; point_index++) {
            /* The loss describes the model before this sample's update. */
            float loss = optimizer_backward(&training, x[point_index],
                    y[point_index]);
            optimizer_update(&training, lr);
            printf("epoch=%zu sample=%zu loss_before=%.9g a=%.9g b=%.9g\n",
                    epoch_index + 1, point_index + 1, loss, model.a, model.b);
        }
    }

    size_t validation_count = sizeof(validation_x) / sizeof(validation_x[0]);
    float validation_sum = 0.0f;

    for (size_t index = 0; index < validation_count; index++) {
        float predicted = regression_forward(&model, validation_x[index]);
        float residual = predicted - validation_y[index];
        validation_sum += residual * residual;
        printf("validation sample=%zu x=%.9g expected=%.9g"
                " predicted=%.9g\n", index + 1, validation_x[index],
                validation_y[index], predicted);
    }

    printf("validation_mse=%.9g\n",
            validation_sum / (float)validation_count);
    putchar('\n');
}


/**
 * @brief Train with one full-batch update per epoch, then validate.
 * @param[in] lr Learning rate for all ten epochs.
 * @note Validation samples are not passed to the optimizer.
 */
static void run_bgd_experiment(float lr) {
    srand(42);
    Regression model = regression();
    Optimizer training = optimizer(&model);
    const float x[] = {1.0f, 3.0f, 5.0f};
    const float y[] = {0.625f, 1.875f, 3.125f};
    const float validation_x[] = {2.0f, 4.0f, 6.0f};
    const float validation_y[] = {1.25f, 2.5f, 3.75f};
    size_t epoch = 10;
    size_t point_count = sizeof(x) / sizeof(x[0]);

    printf("BGD: seed=42 lr=%.9g epochs=%zu initial_a=%.9g"
            " initial_b=%.9g\n", lr, epoch, model.a, model.b);

    for (size_t epoch_index = 0; epoch_index < epoch; epoch_index++) {
        float mse = optimizer_backward_batch(&training, x, y, point_count);
        optimizer_update(&training, lr);
        printf("epoch=%zu mse_before=%.9g a=%.9g b=%.9g\n",
                epoch_index + 1, mse, model.a, model.b);
    }

    size_t validation_count = sizeof(validation_x) / sizeof(validation_x[0]);
    float validation_sum = 0.0f;

    for (size_t index = 0; index < validation_count; index++) {
        float predicted = regression_forward(&model, validation_x[index]);
        float residual = predicted - validation_y[index];
        validation_sum += residual * residual;
        printf("validation sample=%zu x=%.9g expected=%.9g"
                " predicted=%.9g\n", index + 1, validation_x[index],
                validation_y[index], predicted);
    }

    printf("validation_mse=%.9g\n",
            validation_sum / (float)validation_count);
    putchar('\n');
}


/**
 * @brief Compute the full-set MSE without modifying model or gradients.
 * @param[in] model Current regression model.
 * @param[in] x Inputs for the measured set.
 * @param[in] y Targets for the measured set.
 * @param[in] count Positive number of samples.
 * @return Mean squared error for the current model.
 */
static float comparison_mse(Regression *model, const float *x,
        const float *y, size_t count) {
    float sum = 0.0f;

    for (size_t index = 0; index < count; index++) {
        float residual = regression_forward(model, x[index]) - y[index];
        sum += residual * residual;
    }

    return sum / (float)count;
}


/**
 * @brief Run one fixed-initialization, per-sample training comparison.
 * @param[in] name Case label used in machine-readable diagnostics.
 * @param[in] x Training inputs.
 * @param[in] y Training targets.
 * @param[in] validation_x Held-out inputs.
 * @param[in] validation_y Held-out targets.
 * @param[in] count Number of samples in each set.
 * @param[in] slope_only Keep the intercept fixed at zero when true.
 * @param[in] epochs Number of full passes through the training set.
 */
static void run_comparison_sgd(const char *name, const float *x,
        const float *y, const float *validation_x,
        const float *validation_y, size_t count, bool slope_only,
        size_t epochs) {
    Regression model = {.a = -0.5f};
    Optimizer training = optimizer(&model);
    const float lr = 0.01f;
    size_t mse_increases = 0;
    float previous_mse = comparison_mse(&model, x, y, count);

    printf("comparison case=%s method=SGD lr=%.9g epochs=%zu"
            " initial_a=%.9g initial_b=%.9g initial_mse=%.9g\n",
            name, lr, epochs, model.a, model.b, previous_mse);

    for (size_t epoch = 0; epoch < epochs; epoch++) {
        for (size_t sample = 0; sample < count; sample++) {
            optimizer_backward(&training, x[sample], y[sample]);

            if (slope_only) {
                optimizer_update_slope_only(&training, lr);
            } else {
                optimizer_update(&training, lr);
            }

            float mse = comparison_mse(&model, x, y, count);

            if (mse > previous_mse) {
                mse_increases++;
            }

            printf("comparison_step case=%s method=SGD epoch=%zu"
                    " sample=%zu mse_after=%.9g a=%.9g b=%.9g\n",
                    name, epoch + 1, sample + 1, mse, model.a, model.b);
            previous_mse = mse;
        }
    }

    float validation_mse = comparison_mse(&model, validation_x,
            validation_y, count);

    printf("comparison_summary case=%s method=SGD updates=%zu"
            " mse_increases=%zu final_mse=%.9g validation_mse=%.9g"
            " a=%.9g b=%.9g\n\n", name, epochs * count, mse_increases,
            previous_mse, validation_mse, model.a, model.b);
}


/**
 * @brief Run one fixed-initialization, full-batch training comparison.
 * @param[in] name Case label used in machine-readable diagnostics.
 * @param[in] x Training inputs.
 * @param[in] y Training targets.
 * @param[in] validation_x Held-out inputs.
 * @param[in] validation_y Held-out targets.
 * @param[in] count Number of samples in each set.
 * @param[in] slope_only Keep the intercept fixed at zero when true.
 * @param[in] epochs Number of full passes through the training set.
 */
static void run_comparison_bgd(const char *name, const float *x,
        const float *y, const float *validation_x,
        const float *validation_y, size_t count, bool slope_only,
        size_t epochs) {
    Regression model = {.a = -0.5f};
    Optimizer training = optimizer(&model);
    const float lr = 0.01f;
    size_t mse_increases = 0;
    float previous_mse = comparison_mse(&model, x, y, count);

    printf("comparison case=%s method=BGD lr=%.9g epochs=%zu"
            " initial_a=%.9g initial_b=%.9g initial_mse=%.9g\n",
            name, lr, epochs, model.a, model.b, previous_mse);

    for (size_t epoch = 0; epoch < epochs; epoch++) {
        optimizer_backward_batch(&training, x, y, count);

        if (slope_only) {
            optimizer_update_slope_only(&training, lr);
        } else {
            optimizer_update(&training, lr);
        }

        float mse = comparison_mse(&model, x, y, count);

        if (mse > previous_mse) {
            mse_increases++;
        }

        printf("comparison_step case=%s method=BGD epoch=%zu"
                " mse_after=%.9g a=%.9g b=%.9g\n",
                name, epoch + 1, mse, model.a, model.b);
        previous_mse = mse;
    }

    float validation_mse = comparison_mse(&model, validation_x,
            validation_y, count);

    printf("comparison_summary case=%s method=BGD updates=%zu"
            " mse_increases=%zu final_mse=%.9g validation_mse=%.9g"
            " a=%.9g b=%.9g\n\n", name, epochs, mse_increases,
            previous_mse, validation_mse, model.a, model.b);
}


/**
 * @brief Run SGD and BGD against the same training and validation sets.
 * @param[in] name Case label used in machine-readable diagnostics.
 * @param[in] x Training inputs.
 * @param[in] y Training targets.
 * @param[in] validation_x Held-out inputs.
 * @param[in] validation_y Held-out targets.
 * @param[in] count Number of samples in each set.
 * @param[in] slope_only Keep the intercept fixed at zero when true.
 * @param[in] epochs Number of full passes through the training set.
 */
static void run_comparison_pair(const char *name, const float *x,
        const float *y, const float *validation_x,
        const float *validation_y, size_t count, bool slope_only,
        size_t epochs) {
    run_comparison_sgd(name, x, y, validation_x, validation_y, count,
            slope_only, epochs);
    run_comparison_bgd(name, x, y, validation_x, validation_y, count,
            slope_only, epochs);
}


/**
 * @brief Run four original trials, three geometry pairs, and a long C pair.
 * @return Zero after all twelve experiment runs.
 */
int main(void) {
    run_sgd_experiment(0.01f);
    run_bgd_experiment(0.01f);
    run_sgd_experiment(0.1f);
    run_bgd_experiment(0.1f);

    const float x[] = {1.0f, 3.0f, 5.0f};
    const float origin_y[] = {0.625f, 1.875f, 3.125f};
    const float shifted_y[] = {1.625f, 2.875f, 4.125f};
    const float scattered_y[] = {1.625f, 3.125f, 4.125f};
    const float validation_x[] = {2.0f, 4.0f, 6.0f};
    const float origin_validation_y[] = {1.25f, 2.5f, 3.75f};
    const float shifted_validation_y[] = {2.25f, 3.5f, 4.75f};
    size_t count = sizeof(x) / sizeof(x[0]);

    run_comparison_pair("A", x, origin_y, validation_x,
            origin_validation_y, count, true, 100);
    run_comparison_pair("B", x, shifted_y, validation_x,
            shifted_validation_y, count, false, 100);
    run_comparison_pair("C", x, scattered_y, validation_x,
            shifted_validation_y, count, false, 100);
    run_comparison_pair("C1000", x, scattered_y, validation_x,
            shifted_validation_y, count, false, 1000);
    return 0;
}
