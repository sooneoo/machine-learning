/**
 * @file optimizer.h
 * @brief One-sample and full-batch gradient descent for Regression.
 */

#ifndef REGRESSION_OPTIMIZER_H
#define REGRESSION_OPTIMIZER_H

#include <stddef.h>

#include "regression/regression.h"

/** @brief Gradient state tied to a caller-owned regression model. */
typedef struct {
    Regression *regression; /**< Borrowed model; it must outlive this. */
    float ga; /**< Gradient of loss with respect to the slope. */
    float gb; /**< Gradient of loss with respect to the intercept. */
} Optimizer;

/**
 * @brief Bind a model and initialize both stored gradients to zero.
 * @param[in] regression Caller-owned model to optimize later.
 * @return Optimizer that borrows regression without allocating memory.
 * @pre regression remains valid while the optimizer is in use.
 */
Optimizer optimizer(Regression *regression);

/**
 * @brief Compute one sample's squared loss and parameter gradients.
 * @param[in,out] self Optimizer receiving the new gradients.
 * @param[in] x Input distance in kilometers.
 * @param[in] y Target distance in miles.
 * @return Squared residual before the parameter update.
 * @pre self and its model are valid; x and y are finite.
 * @note Replaces ga and gb but does not update the model.
 */
float optimizer_backward(Optimizer *self, float x, float y);

/**
 * @brief Compute full-batch mean squared error and mean gradients.
 * @param[in,out] self Optimizer receiving the batch gradients.
 * @param[in] x Input array with at least count elements.
 * @param[in] y Target array with at least count elements.
 * @param[in] count Number of training pairs in the batch.
 * @return Mean squared error before the parameter update.
 * @pre count > 0; self, its model, and both arrays are valid.
 * @pre Every input and target in the batch is finite.
 * @note Replaces ga and gb in one pass without updating the model.
 */
float optimizer_backward_batch(Optimizer *self, const float *x,
        const float *y, size_t count);

/**
 * @brief Apply the gradients stored by the latest backward call.
 * @param[in,out] self Optimizer whose borrowed model is updated.
 * @param[in] lr Finite learning rate for this update.
 * @pre self and its model are valid; no pointer checks run here.
 * @note Before any backward call the gradients are zero.
 */
void optimizer_update(Optimizer *self, float lr);

/**
 * @brief Apply the stored slope gradient while keeping the intercept fixed.
 * @param[in,out] self Optimizer whose borrowed model is updated.
 * @param[in] lr Finite learning rate for this update.
 * @pre self and its model are valid; no pointer checks run here.
 */
void optimizer_update_slope_only(Optimizer *self, float lr);

#endif
