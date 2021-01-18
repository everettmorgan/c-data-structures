#include "./queue.c"

void queue_insert(queue * q, queue_node * n)
void queue_enqueue(queue * q, queue_node * n);
void queue_dequeue(queue * q, queue_node * n);
void queue_search(queue * q, void * d);
void queue_free(queue * q);
void queue_print(queue * q);