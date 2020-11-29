# Arrays

a collection of elements that are of the same type.

**Pass by reference**

It's important to note that C treats any array as a pointer to the first element in that array.

## Exercises

1. Find the second minimum element of an array ☑
2. First non-repeating integers in an array ☑
3. Merge two sorted arrays ☑
4. Rearrange positive and negative values in an array

The above Exercises have their own sub-directories with compiled binaries for the Mac + a README walking through the algorithm. You'll need to recompile the binaries for your specific system as needed.

## Example code

```c
int numbers[] = { 1, 2, 3, 4, 5 };
numbers[0] -> 1
numbers[3] -> 4

char *name = "Hello"; // ['H', 'e', 'l', 'l', 'o', '\0']
*name -> name[0]
name[0] -> 'H'
name[1] -> 'e'

struct Rectangle rects[] = {{ l: 3, w: 5 }, { l: 4, w: 4 }};
rects[1] -> { l: 4, w: 4 }
```

### Todo

- find more memory efficient algorithms
