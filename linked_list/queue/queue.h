#include "./queue.c"

queue * queue_new(int max_length);
int queue_insert(queue * q, queue_node * n);
int queue_enqueue(queue * q, queue_node * n);
int queue_dequeue(queue * q, queue_node * n);
queue_node * queue_search(queue * q, void * d);
void queue_free(queue * q);
void queue_print(queue * q);