#include "./queue.h"

int main() {
  queue * q = queue_new(5);

  queue_enqueue(q, (char *)'h');
  queue_enqueue(q, (char *)'e');
  queue_enqueue(q, (char *)'l');
  queue_enqueue(q, (char *)'l');
  queue_enqueue(q, (char *)'o');

  queue_print(q);

  node * qn = queue_peek(q);
  printf("head: %p\n", qn);

  queue_dequeue(q);
  queue_dequeue(q);
  queue_dequeue(q);

  queue_print(q);
  queue_free(q);
}