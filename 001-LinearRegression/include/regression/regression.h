#ifndef REGRESSION_REGRESSION_H
#define REGRESSION_REGRESSION_H

typedef struct {
    float a;
    float b;
} Regression;

Regression regression(void);
float regression_forward(Regression *self, float x);

#endif
