/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "./queue.c"

queue * queue_new(int max_length);
int queue_enqueue(queue * q, void * d);
int queue_dequeue(queue * q);
node * queue_peek(queue * q);
void queue_free(queue * q);
void queue_print(queue * q);