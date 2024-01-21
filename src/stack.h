#ifndef SHOOL21_STACK_H_
#define SHOOL21_STACK_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct stack_s stack_t;

struct stack_s {
  char data;
  double value;
  stack_t *prev;
};

void pop(stack_t **stack, char *symb, double *num);
void push(stack_t **stack, char symb, double num);
void reverse_stack(stack_t *old, stack_t **new);
void destroy_stack(stack_t *st);

#endif  // SHOOL21_STACK_H_
