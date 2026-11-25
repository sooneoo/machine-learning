#ifndef REGRESSION_OPTIMIZER_H
#define REGRESSION_OPTIMIZER_H

#include "regression/regression.h"

typedef struct {
    Regression *regression;
    float ga;
    float gb;
} Optimizer;

Optimizer optimizer(Regression *regression);
float optimizer_backward(Optimizer *self, float x, float y);
void optimizer_update(Optimizer *self, float lr);

#endif
