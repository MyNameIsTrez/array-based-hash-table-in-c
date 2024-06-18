# Array-based hash table in C

This [hash table](https://en.m.wikipedia.org/wiki/Hash_table) requires you to have an array of items that you want to hash. `main.c` contains a `persons` array of `person` structs. Each person has a name and an age.

The [pigeonhole principle](https://en.wikipedia.org/wiki/Pigeonhole_principle) tells us that if we have 3 persons, and we're trying to fit them into 2 buckets, at least one bucket must contain more than one person.

In order to fit more than one person in each bucket, each bucket is a linked list.

Usually linked lists are implemented by having every node be `malloc()`ed separately, but `main.c` manages to do it with just has a single, global `buckets` and `chains` array.

I made these arrays are global in order to make the code easier to read, but you could of course just pass them as arguments to the functions.

## How it works

Let's assume that `#define BUCKET_COUNT 420` has been changed from 420 to 2 for a moment.

`main()` then pushes into the `persons` array in this order:
1. `trez`, with an age of 7
2. `john`, with an age of 42
3. `carl`, with an age of 69

The program hashes the person structs based on their names, so let's use an imaginary `hash()` function:
1. `hash("trez")` returns 0
2. `hash("john")` returns 1
3. `hash("carl")` returns 0

Here's how those hashes are used to give `buckets` and `chains` their values, where `nbucket` is `BUCKET_COUNT`, and `<SENTINEL>` is a [sentinel value](https://en.wikipedia.org/wiki/Sentinel_value).

```
Bucket[i] has the value of the last entry that has bucket_index equal to i

 i  buckets[i]  name of first symbol in chain
--  ----------  -----------------------------
 0  3           carl
 1  2           john

One asterisk * indicates the start of a chain

    name =            | hash =      bucket_index =  chain =
 i  persons[i-1].name | hash(name)  hash % nbucket  chains[i]
--  ----------------- | ----------  --------------  ---------
 0  <SENTINEL>        |
 1  trez              |  0          0               0 <-\
 2  john              |  1          1 *             0   |
 3  carl              |  0          0 *             1 --/
```

Let's test this table using `get_person()` its algorithm:

1. `john`:
```
hash("john") = 1
chain starts at buckets[1 % 2] = bucket[1] = 2

persons[2-1].name (= "john") == "john"? yes => "john" found at persons index 1
```

2. `bob`:
```
hash("bob") = 0
chain starts at buckets[0 % 2] = bucket[0] = 3

persons[3-1].name (= "carl") == "bob"? no => chain continues at i=1
persons[1-1].name (= "trez") == "bob"? no => chain continues at i=0
i=0 is the sentinel index, so "bob" is not in the hash table
```

## Running

If you edit the `#define BUCKET_COUNT 420` line to `#define BUCKET_COUNT 1`, you'll see that it still works!

```bash
clear && \
gcc main.c -Wall -Wextra -Werror -Wpedantic -Wfatal-errors -g && \
./a.out
```

## Credits

My explanation is based on flapenguin's great blog post called [ELF: symbol lookup via DT_HASH](https://flapenguin.me/elf-dt-hash).

The hash table implementation is taken from the code Linux uses to perform symbol lookup in ELF files. Its Linux Foundation documentation is [here](https://refspecs.linuxfoundation.org/elf/gabi4+/ch5.dynamic.html#hash). 
