#include "smart_calc.h"

int solve_expression(char *expression, double x, double *answer) {
  int return_code = EXIT_SUCCESS;
  stack_t *stack = NULL;
  return_code = pars(expression, &stack);
  if (!return_code) {
    stack_t *reversed_stack = NULL;
    reverse_stack(stack, &reversed_stack);
    return_code = calc(reversed_stack, x, answer);
    destroy_stack(reversed_stack);
  }

  destroy_stack(stack);
  return return_code;
}

int get_graph(char *expression, double l, double r, double step,
              double coordinates[][2]) {
  if (l >= r) return EXIT_EDGES_ERROR;
  int return_code = EXIT_SUCCESS;
  stack_t *stack = NULL;
  return_code = pars(expression, &stack);
  if (!return_code) {
    stack_t *reversed_stack = NULL;
    reverse_stack(stack, &reversed_stack);
    draw(reversed_stack, l, r, step, coordinates);
    destroy_stack(reversed_stack);
  }

  destroy_stack(stack);
  return return_code;
}
