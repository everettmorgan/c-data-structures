#include<stdio.h>

// Find first non-repeating integers of an array

#define ARRAY_LEN 15
#define PADD      -1

// ff_non_rep (find first non-repeating) : traverses an array until it
// finds the first non-repeating character. ff_non_rep() will return -1
// if no non-repeating integer is found in the given array. ff_non_rep() expects
// incoming arrays to terminate themselves with -1 or the PADD symbolic link.
int ff_non_rep(int a[]) {
  int prev, nrep;

  for (int i=0; i < ARRAY_LEN; i++) {
    if (ARRAY_LEN < 2) nrep = a[i];

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
      if (a[i+1] != a[i] && a[i+1] != prev) {
        nrep = a[i];
        break;
      }
    }

    prev = a[i];
  }

  return nrep;
}

int main() {
  int d[ARRAY_LEN] = {1,1,2,2,22,PADD}; // even
  int t[ARRAY_LEN] = {1,1,1,2,2,2,22,3,3,PADD}; // odd
  int s[ARRAY_LEN] = {22,1,1,2,2,3,PADD}; // start
  int e[ARRAY_LEN] = {1,1,2,2,3,3,22,PADD}; // start

  int dnrep = ff_non_rep(d);
  int tnrep = ff_non_rep(t);
  int snrep = ff_non_rep(s);
  int enrep = ff_non_rep(e);

  printf("dnrep: %d\n", dnrep);
  printf("tnrep: %d\n", tnrep);
  printf("snrep: %d\n", snrep);
  printf("enrep: %d\n", enrep);
}