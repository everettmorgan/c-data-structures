/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <assert.h>
#include "queue.h"

int main(void) {
  int first = 1;
  int second = 2;
  int third = 3;
  queue *q = queue_new(3);

  assert(queue_enqueue(q, &first) == 0);
  assert(queue_enqueue(q, &second) == 0);
  assert(queue_enqueue(q, &third) == 0);
  assert(queue_peek(q)->data == &first);

  assert(queue_dequeue(q) == 0);
  assert(queue_peek(q)->data == &second);
  assert(queue_dequeue(q) == 0);
  assert(queue_peek(q)->data == &third);
  assert(queue_dequeue(q) == 0);
  assert(queue_peek(q) == NULL);
  assert(queue_dequeue(q) == 1);

  queue_free(q);
  return 0;
}
