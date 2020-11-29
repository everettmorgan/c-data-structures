/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "../linked_list.h"

typedef linked_list stack;

stack *new_stack(int max_sz) {
  return new_linked_list(SING_LL, max_sz);
}

void push(stack *stk, int d) {
  insert((linked_list *)stk, d);
}

void pop(stack *stk) {
  delete_head((linked_list *)stk);
}

node *peek(stack *stk) {
  return stk->h;
}

void print_stack(stack *stk) {
  print_linked_list((linked_list *)stk);
}