#include<stdio.h>
#include<limits.h>

#define ARRAY_LEN 15
#define PADD      -1

// Find the second minimum element of an array

int find_sec_low(int a[ARRAY_LEN]) {
  int flw = INT_MAX; // first lowest
  int slw = INT_MAX; // second lowest

  for(int i=0; i < ARRAY_LEN; i++) {
    if (a[i] == PADD) break;
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
  int e[ARRAY_LEN] = {255,64,34,97,86,36,35,42,913,36,PADD};
  int slw = find_sec_low(e);

  printf("lowest: %d\n", slw);
}