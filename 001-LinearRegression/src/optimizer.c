/**
 * @file optimizer.c
 * @brief Loss gradients and in-place parameter updates.
 */

#include "regression/optimizer.h"

Optimizer optimizer(Regression *regression) {
    return (Optimizer) {
        .regression = regression
    };
}


float optimizer_backward(Optimizer *self, float x, float y) {
    float difference = regression_forward(self->regression, x) - y;

    self->ga = 2.0f * difference * x;
    self->gb = 2.0f * difference;
    return difference * difference;
}


/* All residuals use the same model parameters until update. */
float optimizer_backward_batch(Optimizer *self, const float *x,
        const float *y, size_t count) {
    float loss_sum = 0.0f;
    float ga_sum = 0.0f;
    float gb_sum = 0.0f;

    for (size_t index = 0; index < count; index++) {
        float residual = regression_forward(self->regression, x[index])
                - y[index];
        loss_sum += residual * residual;
        ga_sum += residual * x[index];
        gb_sum += residual;
    }

    float inverse_count = 1.0f / (float)count;
    self->ga = 2.0f * inverse_count * ga_sum;
    self->gb = 2.0f * inverse_count * gb_sum;
    return inverse_count * loss_sum;
}


void optimizer_update(Optimizer *self, float lr) {
    self->regression->a -= lr * self->ga;
    self->regression->b -= lr * self->gb;
}
