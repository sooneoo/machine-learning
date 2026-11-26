/**
 * @file main.c
 * @brief Compare sigmoid and ReLU neurons on the AND truth table.
 */

#include "perceptron/optimizer.h"
#include "perceptron/perceptron.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/** @brief Train the sigmoid baseline with full-batch gradient descent. */
static void run_and_experiment(void) {
    srand(42);
    Perceptron model = perceptron();
    Optimizer training = optimizer(&model);
    const float inputs[][PERCEPTRON_INPUT_COUNT] = {
        {0.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f}
    };
    const float targets[] = {0.0f, 0.0f, 0.0f, 1.0f};
    const float lr = 0.2f;
    const size_t epochs = 10000;
    const size_t count = sizeof(inputs) / sizeof(inputs[0]);

    printf("BGD sigmoid: seed=42 lr=%.9g epochs=%zu initial_w0=%.9g"
            " initial_w1=%.9g initial_b=%.9g\n",
            lr, epochs, model.w[0], model.w[1], model.b);

    for (size_t epoch = 0; epoch < epochs; epoch++) {
        float mse = optimizer_backward(&training, inputs, targets, count);

        optimizer_update(&training, lr);

        if (epoch % 100 == 0) {
            printf("epoch=%zu mse_before=%.9g w0=%.9g w1=%.9g b=%.9g\n",
                    epoch + 1, mse, model.w[0], model.w[1], model.b);
        }
    }

    float loss_sum = 0.0f;

    for (size_t index = 0; index < count; index++) {
        float predicted = perceptron_forward(&model, inputs[index]);
        float residual = predicted - targets[index];

        loss_sum += residual * residual;
        printf("training sample=%zu x0=%.9g x1=%.9g"
                " expected=%.9g predicted=%.9g\n", index + 1,
                inputs[index][0], inputs[index][1], targets[index],
                predicted);
    }

    printf("training_mse=%.9g\n\n", loss_sum / (float) count);
}


/** @brief Train a ReLU neuron from the supplied initial model. */
static void run_relu_experiment(const char *initialization, Perceptron model) {
    Optimizer training = optimizer(&model);
    const float inputs[][PERCEPTRON_INPUT_COUNT] = {
        {0.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f}
    };
    const float targets[] = {0.0f, 0.0f, 0.0f, 1.0f};
    const float lr = 0.2f;
    const size_t epochs = 10000;
    const size_t count = sizeof(inputs) / sizeof(inputs[0]);

    training.backward = optimizer_backward_mse_relu;
    printf("BGD ReLU %s: lr=%.9g epochs=%zu initial_w0=%.9g"
            " initial_w1=%.9g initial_b=%.9g\n",
            initialization, lr, epochs, model.w[0], model.w[1], model.b);

    for (size_t epoch = 0; epoch < epochs; epoch++) {
        float mse = optimizer_backward(&training, inputs, targets, count);

        optimizer_update(&training, lr);

        if (epoch % 100 == 0) {
            printf("epoch=%zu mse_before=%.9g w0=%.9g w1=%.9g b=%.9g\n",
                    epoch + 1, mse, model.w[0], model.w[1], model.b);
        }
    }

    float loss_sum = 0.0f;

    for (size_t index = 0; index < count; index++) {
        float predicted = perceptron_forward_relu(&model, inputs[index]);
        float residual = predicted - targets[index];

        loss_sum += residual * residual;
        printf("training sample=%zu x0=%.9g x1=%.9g"
                " expected=%.9g predicted=%.9g\n", index + 1,
                inputs[index][0], inputs[index][1], targets[index],
                predicted);
    }

    printf("training_mse=%.9g\n\n", loss_sum / (float) count);
}


/** @brief Run one sigmoid baseline and two ReLU initializations. */
int main(void) {
    run_and_experiment();

    srand(42);
    run_relu_experiment("seeded", perceptron());
    run_relu_experiment("fixed", (Perceptron) {
        .w = {0.1f, 0.1f},
        .b = 0.1f
    });

    return 0;
}
