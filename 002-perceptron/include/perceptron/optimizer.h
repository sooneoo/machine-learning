/**
 * @file optimizer.h
 * @brief Selectable full-batch loss gradients for one sigmoid neuron.
 */

#ifndef PERCEPTRON_OPTIMIZER_H
#define PERCEPTRON_OPTIMIZER_H

#include <stddef.h>

#include "perceptron/perceptron.h"

/** @brief Optimizer state that borrows a caller-owned Perceptron. */
typedef struct Optimizer Optimizer;

/**
 * @brief Full-batch loss and gradient callback.
 * @param[in,out] self Optimizer receiving the new gradients.
 * @param[in] inputs Input rows; each row has two values.
 * @param[in] targets One target per input row.
 * @param[in] count Positive number of rows.
 * @return Mean batch loss before any parameter update.
 * @pre self, its model, inputs, and targets are valid; count > 0.
 * @pre All inputs, targets, and model parameters are finite.
 */
typedef float (*OptimizerBackward)(Optimizer *self,
        const float inputs[][PERCEPTRON_INPUT_COUNT],
        const float targets[], size_t count);

/** @brief Borrowed model, stored gradients, and selected backward method. */
struct Optimizer {
    Perceptron *model; /**< Borrowed model; caller retains ownership. */
    float gw[PERCEPTRON_INPUT_COUNT]; /**< Mean weight gradients. */
    float gb; /**< Mean bias gradient. */
    OptimizerBackward backward; /**< Selected full-batch method. */
};

/**
 * @brief Bind a model, select MSE backward, and zero gradients.
 * @param[in] model Caller-owned model that outlives the optimizer.
 * @return Value-owned optimizer bound to the model.
 * @pre model remains valid during all optimizer calls.
 */
Optimizer optimizer(Perceptron *model);

/**
 * @brief Compute batch MSE and store its mean parameter gradients.
 * @param[in,out] self Optimizer receiving new gradients.
 * @param[in] inputs Input rows of width PERCEPTRON_INPUT_COUNT.
 * @param[in] targets Target values for the rows.
 * @param[in] count Positive number of training rows.
 * @return Mean squared error before the update.
 * @pre self, its model, and both arrays are valid; count > 0.
 * @pre All input values, targets, and model parameters are finite.
 * @note This function does not change the model parameters.
 */
float optimizer_backward_mse(Optimizer *self,
        const float inputs[][PERCEPTRON_INPUT_COUNT],
        const float targets[], size_t count);

/**
 * @brief Dispatch one full batch through the selected backward method.
 * @param[in,out] self Optimizer with a valid backward callback.
 * @param[in] inputs Input rows of width PERCEPTRON_INPUT_COUNT.
 * @param[in] targets One target per input row.
 * @param[in] count Positive number of rows.
 * @return Loss returned by the selected backward method.
 * @pre self, backward, model, arrays, and rows are valid; count > 0.
 */
static inline float optimizer_backward(Optimizer *self,
        const float inputs[][PERCEPTRON_INPUT_COUNT],
        const float targets[], size_t count) {
    return self->backward(self, inputs, targets, count);
}

/**
 * @brief Apply the most recently stored gradients to the model.
 * @param[in,out] self Optimizer bound to a live model.
 * @param[in] lr Finite learning rate.
 * @pre self and its model are valid; lr is finite.
 * @note Before the first backward call all gradients are zero.
 */
void optimizer_update(Optimizer *self, float lr);

#endif
