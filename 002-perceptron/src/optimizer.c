/**
 * @file optimizer.c
 * @brief Full-batch MSE gradients and parameter updates.
 */

#include "perceptron/optimizer.h"

Optimizer optimizer(Perceptron *model) {
    return (Optimizer) {
        .model = model,
        .backward = optimizer_backward_mse
    };
}


float optimizer_backward_mse(Optimizer *self,
        const float inputs[][PERCEPTRON_INPUT_COUNT],
        const float targets[], size_t count) {
    float loss_sum = 0.0f;
    float gw0_sum = 0.0f;
    float gw1_sum = 0.0f;
    float gb_sum = 0.0f;

    for (size_t index = 0; index < count; index++) {
        float predicted = perceptron_forward(self->model, inputs[index]);
        float residual = predicted - targets[index];
        float delta = residual * predicted * (1.0f - predicted);

        loss_sum += residual * residual;
        gw0_sum += delta * inputs[index][0];
        gw1_sum += delta * inputs[index][1];
        gb_sum += delta;
    }

    float scale = 2.0f / (float) count;

    self->gw[0] = scale * gw0_sum;
    self->gw[1] = scale * gw1_sum;
    self->gb = scale * gb_sum;
    return loss_sum / (float) count;
}


void optimizer_update(Optimizer *self, float lr) {
    self->model->w[0] -= lr * self->gw[0];
    self->model->w[1] -= lr * self->gw[1];
    self->model->b -= lr * self->gb;
}


float optimizer_backward_mse_relu(Optimizer *self,
        const float inputs[][PERCEPTRON_INPUT_COUNT],
        const float targets[], size_t count) {
    float loss_sum = 0.0f;
    float gw0_sum = 0.0f;
    float gw1_sum = 0.0f;
    float gb_sum = 0.0f;

    for (size_t index = 0; index < count; index++) {
        float predicted = perceptron_forward_relu(self->model, inputs[index]);
        float residual = predicted - targets[index];
        float delta = predicted > 0.0f ? residual : 0.0f;

        loss_sum += residual * residual;
        gw0_sum += delta * inputs[index][0];
        gw1_sum += delta * inputs[index][1];
        gb_sum += delta;
    }

    float scale = 2.0f / (float) count;

    self->gw[0] = scale * gw0_sum;
    self->gw[1] = scale * gw1_sum;
    self->gb = scale * gb_sum;
    return loss_sum / (float) count;
}
