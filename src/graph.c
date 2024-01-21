#include "graph.h"

void draw(stack_t *stack, double l, double r, double step, double coords[][2]) {
  int i = 0;
  for (double x = l; x <= r; x += step) {
    double y = 0;
    int rc = calc(stack, x, &y);
    coords[i][0] = x;
    coords[i][1] = rc ? NAN : y;
    i++;
  }
}
