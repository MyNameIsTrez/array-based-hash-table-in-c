# Hash table in C

The [pigeonhole principle](https://en.wikipedia.org/wiki/Pigeonhole_principle) tells us that if we have 3 persons, and we're trying to fit them into 2 buckets, at least one bucket must contain more than one person.

In order to fit more than one person in each bucket, each bucket is a linked list.

Usually linked lists are implemented by having every node be `malloc()`ed separately, but `main.c` manages to do it with just has a single, global `buckets` and `chains` array.

I made these arrays are global in order to make the code easier to read, but you could of course just pass them as arguments to the functions.

The hash table requires you to have an array of items that you want to hash. `main.c` contains a `persons` array of `person`. Each person has a name and an age.

## How it works

Let's assume that `#define BUCKET_COUNT 420` has been changed from 420 to 2 for a moment.

`main()` then pushes into the `persons` array in this order:
1. `trez`, with an age of 7
2. `john`, with an age of 42
3. `carl`, with an age of 69

The program hashes the person structs based on their names, so let's assume a simplified `hash()` function:
1. `hash("trez")` return 0
2. `hash("john")` return 1
3. `hash("carl")` return 0

Here's what we are left with:

Bucket[i] always has the value of the last entry that has `hash % nbucket` equal to `i`

```sh
 i  bucket[i]  name of first symbol in chain
--  ---------  -----------------------------
 0  3          carl
 1  2          john

One asterisk * indicates the start of a chain

    name =          | hash =      bucket_index =
 i  persons[i].name | hash(name)  hash % nbucket  graph
--  --------------- | ----------  --------------  -----
 0  <SENTINEL>      |
 1  trez            |  0          0               0 <-\
 2  john            |  1          1 *             0   |
 3  carl            |  0          0 *             1 --/
```

## Running

If you edit the `#define BUCKET_COUNT 420` line to `#define BUCKET_COUNT 1`, you'll see that it still works!

```bash
clear && \
gcc main.c -Wall -Wextra -Werror -Wpedantic -Wfatal-errors -g && \
./a.out
```

## Further reading

The hash table implementation is taken from the code Linux uses to perform symbol lookup in ELF files. The Linux Foundation documentation is [here](https://refspecs.linuxfoundation.org/elf/gabi4+/ch5.dynamic.html#hash), and flapenguin explains it well [here](https://flapenguin.me/elf-dt-hash).
