/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "../linked_list.h"

typedef struct Shape {
    int w, l;
} Shape;

int shape_equality(Shape * a, Shape * b) { return a->l == b->l && a->w == b->w; };

int main() {
  linked_list *ll = linked_list_new(1);
  linked_list_equality_fn(ll, shape_equality);

  Shape square = { 5, 5 };
  Shape rectangle = { 5, 4 };
  Shape rectangle2 = { 6, 7 };
  Shape rectangle3 = { 12, 23 };

  linked_list_insert(ll, linked_list_node_new(&square));
  linked_list_insert(ll, linked_list_node_new(&rectangle));
  linked_list_insert(ll, linked_list_node_new(&rectangle2));
  linked_list_insert(ll, linked_list_node_new(&rectangle3));
  linked_list_print(ll);

  linked_list_delete_value(ll, &(Shape){5,5});
   linked_list_delete(ll, linked_list_find(ll, &rectangle));
  linked_list_print(ll);

  linked_list_free(ll);
  return 0;
}
