/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "stack.h"

int main() {
  stack * stk = stk_new(5);

  char a = 'h';
  char b = 'e';
  char c = 'l';
  char d = 'l';
  char e = 'o';

  stack_push(stk, &a);
  stack_push(stk, &b);
  stack_push(stk, &c);
  stack_push(stk, &d);
  stack_push(stk, &e);

  stack_print(stk);

  stack_pop(stk);
  stack_pop(stk);
  stack_pop(stk);

  node * h = stack_peek(stk);
  printf("head: %p\n\n", h);

  stack_print(stk);
  stack_free(stk);
}
