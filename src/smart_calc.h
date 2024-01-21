#ifndef __SMART_CALC_H__
#define __SMART_CALC_H__

#include <stdio.h>
#include <stdlib.h>

#include "bonus_part.h"
#include "expression_parsing.h"
#include "graph.h"

#define EXIT_EDGES_ERROR 40

int solve_expression(char *expression, double x, double *answer);
int get_graph(char *expression, double l, double r, double step,
              double coordinates[][2]);

#endif  // __SMART_CALC_H__
