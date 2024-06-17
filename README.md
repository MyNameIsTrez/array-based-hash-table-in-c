# Hash table in C

This hash table implementation is taken from the code Linux uses to perform symbol lookup in ELF files. The Linux Foundation explains it [here](https://refspecs.linuxfoundation.org/elf/gabi4+/ch5.dynamic.html#hash), and flapenguin [here](https://flapenguin.me/elf-dt-hash).

The algorithm requires an array of items; `main.c` stores an array of person structs that have a name and age field. It decides how many buckets to use based on the number of items in the array, so you shouldn't add or remove items from the array once you've created the hash table.

## Running

If you edit the `#define BUCKET_COUNT 420` line to `#define BUCKET_COUNT 1`, you'll see that it still works!

```bash
clear && \
gcc main.c -Wall -Wextra -Werror -Wpedantic -Wfatal-errors -g && \
./a.out
```
