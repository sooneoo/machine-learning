/**
 * @file regression.h
 * @brief Affine regression model and forward prediction.
 */

#ifndef REGRESSION_REGRESSION_H
#define REGRESSION_REGRESSION_H

/** @brief Affine model with slope and intercept stored by value. */
typedef struct {
    float a; /**< Slope in miles per kilometer. */
    float b; /**< Intercept in miles. */
} Regression;

/**
 * @brief Draw two initial parameters from the current C random stream.
 *
 * @return A value-owned model with both parameters in [-1, 1].
 * @note Call srand before this function for a repeatable run on the same
 *       C runtime. No dynamic memory is allocated.
 */
Regression regression(void);

/**
 * @brief Evaluate the affine model at one input.
 * @param[in] self Pointer to a live model.
 * @param[in] x Input distance in kilometers.
 * @return Predicted distance in miles, computed as a * x + b.
 * @pre self is valid; x and both model parameters are finite.
 * @note This function does not modify the model.
 */
float regression_forward(Regression *self, float x);

#endif
