#include<stdio.h>

// 2. Find first non-repeating integers of an array

#define PADD      -1

// ff_non_rep (find first non-repeating) : traverses an array until it
// finds the first non-repeating character or PADD terminal. ff_non_rep() will
// return -1 if no non-repeating integer is found in the given array.
// ff_non_rep() expects incoming arrays to terminate themselves with -1 or the
// PADD symbolic link.
int ff_non_rep(int a[]) {
  int prev, nrep;

  for (int i=0; a[i] != PADD; i++) {
    // empty or PADD-only array
    if (a[i] == PADD) {
      nrep = -1;
      break;
    }

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
  int d[] = {1,1,2,2,22,PADD}; // even
  int t[] = {1,1,1,2,2,2,22,3,3,PADD}; // odd
  int s[] = {22,1,1,2,2,3,PADD}; // start
  int e[] = {1,1,2,2,3,3,22,PADD}; // end

  int dnrep = ff_non_rep(d);
  int tnrep = ff_non_rep(t);
  int snrep = ff_non_rep(s);
  int enrep = ff_non_rep(e);

  printf("dnrep: %d\n", dnrep);
  printf("tnrep: %d\n", tnrep);
  printf("snrep: %d\n", snrep);
  printf("enrep: %d\n", enrep);
}