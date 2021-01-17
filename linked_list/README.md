# Linked Lists

## **Types**

### Singly Linked List

| nodes are connected in one direction

```c
node -> node -> node -> node -> node

node {
  pointer to next;
}
```

### Double Linked List

| nodes are connected in both directions

```c
node <-> node <-> node <-> node <-> node

node {
  pointer to next;
  pointer to prev;
}
```

### Circurlar Linked List

| nodes are connected in one direction with the tail connect to the for a continuous reference

```c
node -> node -> node -> node -> node
^----<---------<----------<-------^

generic node {
  pointer to next;
}

tail_node {
  pointer to head;
}
```

## **Uses**

### Sentinals

Some linked list implementations use sential nodes to pad the start and finish of a newly created linked list. This ensures that a list _always_ has a head a tail, and it decreases the bound check complexity.

### Hash linking

Linked Lists are used to solve for hash map collisions. Hash maps will intake some data and compute it into a unique index to store that input in a presized array. Collision occur when two inputs produce the same hash index. To solve for this, linked lists are created when a collision is detected.

Hash maps have instant look up times (O(1)) when all of the elements are stored uniformily in the presized array. Using linked lists to solve for collisions does impact performance since linked list traversals are O(n). Although, depending on the number of nodes in the list, this may be negligible.

## **Disadvantages**

- Linked lists are naturally larger than arrays since each node stores a reference to another node(s).
- Linked lists are inherently sequential as they're traversed from start to finish (O(n)) which means they're slower than a normal array (O(1)).
- Linked list nodes are stored 'noncontiguously' which basically means not in order, in memory. This is important when talking about the cpu cache / ram tango.

### Links

- [Wikipedia](https://en.wikipedia.org/wiki/Linked_list)

### Todo

- Implement Knuth's DLX (dancing links + algorithm X)
