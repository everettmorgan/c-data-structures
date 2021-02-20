/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "./queue.h"

int main() {
  queue * q = queue_new(5);

  queue_enqueue(q, (char *)'h');
  queue_enqueue(q, (char *)'e');
  queue_enqueue(q, (char *)'l');
  queue_enqueue(q, (char *)'l');
  queue_enqueue(q, (char *)'o');

  node * qn = queue_peek(q);
  printf("head: %p\n", qn);
  queue_print(q);

  queue_dequeue(q);
  queue_dequeue(q);
  queue_dequeue(q);

  node * qnn = queue_peek(q);
  printf("head: %p\n", qnn);
  queue_print(q);

  queue_free(q);
}