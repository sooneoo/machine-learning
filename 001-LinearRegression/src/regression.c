/**
 * @file regression.c
 * @brief Seeded parameter initialization and affine prediction.
 */

#include "regression/regression.h"

#include <stdlib.h>

Regression regression(void) {
    /* Draw first so a and b have a defined order within a seeded run. */
    int random_a = rand();
    int random_b = rand();

    return (Regression) {
        .a = 2.0f * (float) random_a / (float) RAND_MAX - 1.0f,
        .b = 2.0f * (float) random_b / (float) RAND_MAX - 1.0f
    };
}


float regression_forward(Regression *self, float x) {
    return self->a * x + self->b;
}
