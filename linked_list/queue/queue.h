#include "./queue.c"

queue * queue_new(int max_length);
int queue_enqueue(queue * q, void * d);
int queue_dequeue(queue * q);
queue_node * queue_peek(queue * q);
void queue_free(queue * q);
void queue_print(queue * q);