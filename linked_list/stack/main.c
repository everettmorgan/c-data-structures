/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "stack.h"

int main() {
  stack *s = stack_new(5);

  char a = 'h';
  char b = 'e';
  char c = 'l';
  char d = 'l';
  char e = 'o';

  stack_push(s, &a);
  stack_push(s, &b);
  stack_push(s, &c);
  stack_push(s, &d);
  stack_push(s, &e);

  stack_print(s);

  stack_pop(s);
  stack_pop(s);
  stack_pop(s);

  node *h = stack_peek(s);
  printf("head: %p\n\n", h);

  stack_print(s);
  stack_free(s);
}
