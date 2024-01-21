#ifndef __EXPRESSION_CALCULATION_H__
#define __EXPRESSION_CALCULATION_H__

#include <math.h>
#define PI 3.14159265358979323846

#include "stack.h"

#define eps 1e-7

#define EXIT_POW_ZERO_NEG_EXP_ERROR 10
// The power of zero is undefined for a negative exponent

#define EXIT_ZERO_DIVISION_ERROR 11
// Division by zero is undefined

#define EXIT_NEG_ROOT_ERROR 12
// Taking the root of a negative number is not allowed

#define EXIT_INF_TAN_ERROR 13
// At this point the tangent is undefined

#define EXIT_NON_POS_LOG_ERROR 14
// The logarithm is undefined for non-positive numbers

#define EXIT_ARCCOS_RANGE_ERROR 15
// Inverse cosine is undefined for values outside [-1, 1]

#define EXIT_ARCSIN_RANGE_ERROR 16
// Inverse cosine is undefined for values outside [-1, 1]

#define EXIT_EXPR_PARSING_ERROR 20

int calc(stack_t *stack, double x, double *answer);

#endif  // __EXPRESSION_CALCULATION_H__
