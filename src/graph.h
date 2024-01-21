#ifndef __GRAPH_H__
#define __GRAPH_H__

#define pi acos(-1)
#define eps 1e-7

#define WIDTH 80
#define HEIGHT 25

#include <math.h>
#include <stdio.h>

#include "expression_calculation.h"
#include "stack.h"

void draw(stack_t *stack, double l, double r, double step, double coords[][2]);

#endif  // __GRAPH_H__
