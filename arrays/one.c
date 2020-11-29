#include<stdio.h>
#include<limits.h>

// Find the second minimum element of an array

int main() {
  int e[] = {255,64,34,97,86,36,35,42,913,36};

  int cl = INT_MAX;
  int nl = INT_MAX;

  for(int i=0; i < 10; i++) {
    if (e[i] < cl) {
      nl = cl;
      cl = e[i];
    } else if (e[i] < nl) {
      nl = e[i];
    }
  }

  printf("lowest: %d second lowest: %d\n", cl, nl);
}