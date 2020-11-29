#include<stdio.h>

// 2. Find first non-repeating integers of an array

#define PADD      -1

// ff_non_rep (find first non-repeating) : traverses an array until it
// finds the first non-repeating character or PADD terminal. ff_non_rep() will
// return -1 if no non-repeating integer is found in the given array.
// ff_non_rep() expects incoming arrays to terminate themselves with -1 or the
// PADD symbolic link. Arrays without a PADD termination will result in undefined
// behavior and incorrect return results.
int ff_non_rep(int a[]) {
  int prev, nrep = -1;

  for (int i=0; a[i] != PADD; i++) {
    // return array if there's only 1 element
    if (a[i+1] == PADD) {
      nrep = a[i];
      break;
    }

    // start of the array (setup)
    if (i == 0) {
      prev = a[i]; i++;
      if (a[i] != prev) {
        nrep = prev;
        break;
      }
    }

    // end of the array (no match)
    if (a[i] == PADD) {
      nrep = -1;
      break;
    }

    // main check for all other elements
    if (a[i] != prev) {
      if (a[i] != PADD) {
        if (a[i+1] != prev && a[i+1] != a[i]) {
          nrep = a[i];
          break;
        }
      }
    }
    prev = a[i];
  }
  return nrep;
}

int main() {
  printf("== valid ===\n");
  int o1[] = {22,-1}; // -1 terminal
  int o2[] = {22,PADD}; // one
  int d[] = {1,1,2,2,22,PADD}; // even
  int t[] = {1,1,1,2,2,2,22,3,3,PADD}; // odd
  int s[] = {22,1,1,2,2,3,PADD}; // start
  int e[] = {1,1,2,2,3,3,22,PADD}; // end

  int o1nrep = ff_non_rep(o1);
  int o2nrep = ff_non_rep(o2);
  int dnrep = ff_non_rep(d);
  int tnrep = ff_non_rep(t);
  int snrep = ff_non_rep(s);
  int enrep = ff_non_rep(e);

  printf("o1nrep: %d\n", o1nrep);
  printf("o2nrep: %d\n", o2nrep);
  printf("dnrep: %d\n", dnrep);
  printf("tnrep: %d\n", tnrep);
  printf("snrep: %d\n", snrep);
  printf("enrep: %d\n", enrep);

  printf("\n== invalid ===\n");
  int e1[] = {PADD}; // padd_only
  int e2[] = {}; // zero_elem
  int e3[] = {22}; // no_padd

  int e1res = ff_non_rep(e1);
  int e2res = ff_non_rep(e2);
  int e3res = ff_non_rep(e3);

  printf("padd_only: %d\n", e1res);
  printf("zero_elem: %d\n", e2res);
  printf("no padd: %d\n", e3res);
}