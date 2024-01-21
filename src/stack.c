#include "stack.h"

void push(stack_t **stack, char symb, double num) {
  stack_t *temp = (stack_t *)malloc(sizeof(stack_t));
  temp->data = symb;
  temp->value = num;
  temp->prev = *stack;
  *stack = temp;
}

void pop(stack_t **stack, char *symb, double *num) {
  stack_t *temp = (*stack)->prev;
  *symb = (*stack)->data;
  *num = (*stack)->value;
  free(*stack);
  *stack = temp;
}

void reverse_stack(stack_t *old, stack_t **new) {
  stack_t *temp = old;
  while (temp != NULL) {
    push(new, temp->data, temp->value);
    temp = temp->prev;
  }
}

void delete_element(stack_t *elem) {
  stack_t *tmp = elem->prev;
  free(elem);
  if (tmp) delete_element(tmp);
}

void destroy_stack(stack_t *st) {
  if (!st) return;
  if (st->prev) delete_element(st->prev);
  free(st);
}
