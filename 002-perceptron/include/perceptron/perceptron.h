/**
 * @file perceptron.h
 * @brief One neuron with two compile-time inputs and selectable activation.
 */

#ifndef PERCEPTRON_PERCEPTRON_H
#define PERCEPTRON_PERCEPTRON_H

/** @brief Input width fixed for this experiment at compile time. */
enum {
    PERCEPTRON_INPUT_COUNT = 2
};

/** @brief Value-owned weights and bias for one neuron. */
typedef struct {
    float w[PERCEPTRON_INPUT_COUNT]; /**< Input weights. */
    float b; /**< Bias added to the weighted sum. */
} Perceptron;

/**
 * @brief Draw both weights and the bias from the current C random stream.
 * @return Value-owned model with every parameter in [-1, 1].
 * @note Seed rand() in the Runner before calling this constructor.
 * @note The C runtime determines the exact random sequence.
 */
Perceptron perceptron(void);

/**
 * @brief Evaluate the sigmoid of the weighted input sum.
 * @param[in] self Live model; the function does not modify it.
 * @param[in] x Exactly two finite input values.
 * @return Sigmoid output for the input pair.
 * @pre self and x are valid; all model parameters and inputs are finite.
 */
float perceptron_forward(Perceptron *self,
        const float x[PERCEPTRON_INPUT_COUNT]);

/**
 * @brief Evaluate ReLU of the weighted input sum.
 * @param[in] self Live model; the function does not modify it.
 * @param[in] x Exactly two finite input values.
 * @return Nonnegative, unbounded ReLU output for the input pair.
 * @pre self and x are valid; all model parameters and inputs are finite.
 */
float perceptron_forward_relu(Perceptron *self,
        const float x[PERCEPTRON_INPUT_COUNT]);

#endif
