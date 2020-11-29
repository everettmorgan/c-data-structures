#include<stdio.h>
#include<limits.h>

// 1. Find the second minimum element of an array

#define PADD      -1

// find_sec_low : traverses an array of any length until it reaches a PADD
// terminal and returns the second lowest element in the array. find_sec_low()
// expects incoming arrays to terminate themselves using the provided PADD def.
int find_sec_low(int a[]) {
  int flw = INT_MAX; // first lowest
  int slw = INT_MAX; // second lowest

  for(int i=0; a[i] != PADD; i++) {
    if (a[i] < flw) {
      slw = flw;
      flw = a[i];
    } else if (a[i] < slw) {
      slw = a[i];
    }
  }
  return slw;
}

int main() {
  int e[] = {1,8,255,64,22,34,97,86,36,35,42,913,36,PADD};
  int slw = find_sec_low(e);

  printf("lowest: %d\n", slw);
}