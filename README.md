# Hash table in C

This hash table implementation is taken from the code Linux uses to perform symbol lookup in ELF files. The Linux Foundation explains it [here](https://refspecs.linuxfoundation.org/elf/gabi4+/ch5.dynamic.html#hash), and flapenguin [here](https://flapenguin.me/elf-dt-hash).

## Running

```bash
clear && \
gcc main.c -Wall -Wextra -Werror -Wpedantic -Wfatal-errors -g && \
./a.out
```
