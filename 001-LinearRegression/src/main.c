/**
 * @file main.c
 * @brief Run SGD and BGD experiments and evaluate held-out points.
 */

#include "regression/optimizer.h"
#include "regression/regression.h"

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
 * @brief Compare SGD and BGD at two learning rates with seed 42.
 * @return Zero after all four experiment runs.
 */
int main(void) {
    run_sgd_experiment(0.01f);
    run_bgd_experiment(0.01f);
    run_sgd_experiment(0.1f);
    run_bgd_experiment(0.1f);
    return 0;
}
