#include "stack.h"

int main() {
  stack s = {};
  push(&s, 'h');
  push(&s, 'e');
  push(&s, 'l');
  push(&s, 'l');
  push(&s, 'o');

  print_stack(&s);

  pop(&s);
  pop(&s);
  pop(&s);

  struct node *h = peek(&s);
  printf("head: %p\n\n", h);

  print_stack(&s);
}