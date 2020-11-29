# Stack (Singly Linked List)

**What is a stack?**

A Stack is a singly linked list with slightly different actions to create a FILO or LIFO queue. A Stack _can_ have a max_limit which returns an error or IS_FULL definition.

**When would you use it?**

Stacks are typically used to represent FILO (first in last out) or LIFO (last in first out) queues. These queues can be thought of as a stack of dishes. You stack up plates from bottom to top (FILO) and unload them from top to bottom (LIFO).

We've all interacted with a stack without even realizing it. The 'undo' function is exactly that — a stack of previous actions. At a defined interval, your program state is saved and push onto a stack. When you undo or CTRL+Z you're performing a pop() on that stack.

---

## Required Methods

These methods are **_required_** to implement a stack.

- **push**: add an element to the head of the list
- **pop** : remove the head element from the list

## Optional Methods

- **peek** : return a pointer to the head node
- **find** : traverses the linked list until a node meets the criteria
- **reverse** : reverse the order of the list (head -> tail, tail -> head)
- **isEmpty** : returns whether or not the list is empty
- **isFull** : returns whether or not a list is full

---

## Example code

```c
s = stack(3);

// [action] -> [stack]
push(s, 1) -> {1};
push(s, 2) -> {2,1};
push(s, 3) -> {3,2,1};
pop(s)     -> {2,1};
pop(s)     -> {1};
pop(s)     -> {};
```
