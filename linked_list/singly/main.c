/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "../linked_list.h"

typedef struct Shape {
    int w, l;
} Shape;

int shape_equality(Shape * a, Shape * b) {
  return a->l == b->l && a->w == b->w;
}

int main() {
  linked_list *ll = ll_new(0);
  ll_eq_fn(ll, shape_equality);

  Shape square = { 5, 5 };
  Shape rectangle = { 5, 4 };
  Shape rectangle2 = { 6, 7 };
  Shape rectangle3 = { 12, 23 };

  ll_insert(ll, &square);
  ll_insert(ll, &rectangle);
  ll_insert(ll, &rectangle2);
  ll_insert(ll, &rectangle3);
  ll_print(ll);

  ll_del_v(ll, &square);
  ll_del(ll, ll_find(ll, &rectangle));
  ll_print(ll);

  ll_free(ll);
  return 0;
}
