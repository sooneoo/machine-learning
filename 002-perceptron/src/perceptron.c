/**
 * @file perceptron.c
 * @brief Random initialization and direct sigmoid and ReLU inference.
 */

#include "perceptron/perceptron.h"

#include <math.h>
#include <stdlib.h>

Perceptron perceptron(void) {
    /* Draw first so a fixed seed fixes the parameter order. */
    int random_w0 = rand();
    int random_w1 = rand();
    int random_b = rand();

    return (Perceptron) {
        .w = {
            2.0f * (float) random_w0 / (float) RAND_MAX - 1.0f,
            2.0f * (float) random_w1 / (float) RAND_MAX - 1.0f
        },
        .b = 2.0f * (float) random_b / (float) RAND_MAX - 1.0f
    };
}


float perceptron_forward(Perceptron *self,
        const float x[PERCEPTRON_INPUT_COUNT]) {
    float z = self->w[0] * x[0] + self->w[1] * x[1] + self->b;

    return 1.0f / (1.0f + expf(-z));
}


float perceptron_forward_relu(Perceptron *self,
        const float x[PERCEPTRON_INPUT_COUNT]) {
    float z = self->w[0] * x[0] + self->w[1] * x[1] + self->b;

    return z > 0.0f ? z : 0.0f;
}
