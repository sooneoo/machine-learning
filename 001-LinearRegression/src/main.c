#include "regression/optimizer.h"
#include "regression/regression.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static void run_sgd_experiment(float lr) {
    srand(42);
    Regression model = regression();
    Optimizer training = optimizer(&model);
    const float x[] = {1.0f, 3.0f, 5.0f};
    const float y[] = {0.625f, 1.875f, 3.125f};
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

    putchar('\n');
}


static void run_bgd_experiment(float lr) {
    srand(42);
    Regression model = regression();
    Optimizer training = optimizer(&model);
    const float x[] = {1.0f, 3.0f, 5.0f};
    const float y[] = {0.625f, 1.875f, 3.125f};
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

    putchar('\n');
}


int main(void) {
    run_sgd_experiment(0.01f);
    run_bgd_experiment(0.01f);
    run_sgd_experiment(0.1f);
    run_bgd_experiment(0.1f);
    return 0;
}
