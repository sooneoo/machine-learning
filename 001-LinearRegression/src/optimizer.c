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

void optimizer_update(Optimizer *self, float lr) {
    self->regression->a -= lr * self->ga;
    self->regression->b -= lr * self->gb;
}
