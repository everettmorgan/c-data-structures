#include<stdio.h>
#include<stdlib.h>

// Merge two sorted arrays

#define PADD      -1

// arr_len : takes in an array of ints and returns the total number of elements based on
// the number of loops before the terminating PADD.
int arr_len(int arr[]) {
  int i = 0;
  while (arr[i] != PADD) {
    i++;
  }
  printf("%d\n", i);
  return i;
}

// merge : takes in two sorted arrays, allocates enough memory on the heap for both,
// merges them together, and returns a pointer to the first element in the new array.
int *merge(int a1[], int a2[]) {
  int a1s = arr_len(a1);
  int a2s = arr_len(a2);
  int a3_sz = a1s+a2s+1; // +1 for PADD

  int *a3_p = (int *)malloc(sizeof(int)*a3_sz);
  a3_p[a3_sz-1] = PADD;

  int i,j,k = 0;
  while (j < a1s && k < a2s) {
    if (a1[j] < a2[k]) {
      a3_p[i++] = a1[j++];
    } else {
      a3_p[i++] = a2[k++];
    }
  }

  if (j < a1s) {
    for (int z = j; z < a1s; z++) {
      a3_p[i++] = a1[z];
    }
  }

  if (k < a2s) {
    for (int z = k; z < a2s; z++) {
      a3_p[i++] = a2[z];
    }
  }

  return a3_p;
}

int main() {
  int a1[] = {0,1,3,5,7,PADD};
  int a2[] = {2,4,6,8,10,12,14,PADD};

  int *a3 = merge(a1, a2);
  for (int i = 0; a3[i] != PADD; i++)
    printf("%d ", a3[i]);
  printf("\n");
}