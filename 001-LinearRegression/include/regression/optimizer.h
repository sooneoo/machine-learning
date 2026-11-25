#ifndef REGRESSION_OPTIMIZER_H
#define REGRESSION_OPTIMIZER_H

#include <stddef.h>

#include "regression/regression.h"

typedef struct {
    Regression *regression;
    float ga;
    float gb;
} Optimizer;

Optimizer optimizer(Regression *regression);
float optimizer_backward(Optimizer *self, float x, float y);
float optimizer_backward_batch(Optimizer *self, const float *x,
        const float *y, size_t count);
void optimizer_update(Optimizer *self, float lr);

#endif
