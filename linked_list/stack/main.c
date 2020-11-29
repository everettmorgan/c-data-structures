/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

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

  node *h = peek(&s);
  printf("head: %p\n\n", h);

  print_stack(&s);
}