/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "./stack.c"

#ifdef STK_ENABLE_SHRTHND

#define stk(__cap) stack * stk = stack_new(__cap)

#define stk_push(__v) stack_push(stk, __v)

#define stk_pop() stack_pop(stk)

#define stk_peek() stack_peek(stk)

#define stk_print() stack_print(stk)

#define stk_free() stack_free(stk)

#else

stack * stack_new(int max_length);

int stack_push(stack * stk, void * data);

int stack_pop(stack * stk);

node * stack_peek(stack * stk);

void stack_print(stack * stk);

void stack_free(stack * stk);

#endif // STK_ENABLE_SHRTHND