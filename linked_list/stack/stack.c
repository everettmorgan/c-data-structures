#include "../linked_list/linked_list.h"

typedef linked_list_node stack_node;

typedef struct stack {
    linked_list *ll;
    int max_length;
    int length;
} stack;

stack *stack_new(int max_length) {
    stack *s = malloc(sizeof(stack));
    s->ll = linked_list_new(0);
    s->max_length = max_length;
    return s;
}

void stack_push(stack *stk, void *data) {
    linked_list_insert(stk->ll, linked_list_node_new(&data));
}

void stack_pop(stack *stk) {
    linked_list_delete(stk->ll, stk->ll->head);
}

linked_list_node *stack_peek(stack *stk) {
    return stk->ll->head;
}

void stack_print(stack *stk) {
    linked_list_print(stk->ll);
}

void stack_free(stack *stk) {
    linked_list_free(stk->ll);
    free(stk);
}