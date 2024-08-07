#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PERSONS 420420

struct person {
	char *name;
	uint32_t age;
};

static struct person persons[MAX_PERSONS];
static size_t persons_size;

static uint32_t buckets[MAX_PERSONS];
static uint32_t chains[MAX_PERSONS];

// From https://sourceware.org/git/?p=binutils-gdb.git;a=blob;f=bfd/elf.c#l193
static uint32_t elf_hash(const char *namearg) {
	uint32_t h = 0;
	for (const unsigned char *name = (const unsigned char *) namearg; *name; name++) {
		h = (h << 4) + *name;
		h ^= (h >> 24) & 0xf0;
	}
	return h & 0x0fffffff;
}

static struct person *get_person(char *name) {
	uint32_t i = buckets[elf_hash(name) % persons_size];

	while (1) {
		if (i == UINT32_MAX) {
			return NULL;
		}

		if (strcmp(name, persons[i].name) == 0) {
			break;
		}

		i = chains[i];
	}

	return persons + i;
}

static void hash_persons(void) {
	memset(buckets, UINT32_MAX, persons_size * sizeof(uint32_t));

	for (size_t i = 0; i < persons_size; i++) {
		uint32_t bucket_index = elf_hash(persons[i].name) % persons_size;

		chains[i] = buckets[bucket_index];

		buckets[bucket_index] = i;
	}
}

static void push_person(char *name, uint32_t age) {
	if (persons_size >= MAX_PERSONS) {
		fprintf(stderr, "There are more than %d persons, exceeding MAX_PERSONS\n", MAX_PERSONS);
		exit(EXIT_FAILURE);
	}
	persons[persons_size++] = (struct person){ .name = name, .age = age };
}

int main(void) {
	push_person("trez", 7);
	push_person("john", 42);
	push_person("carl", 69);

	hash_persons();

	printf("%d\n", get_person("trez")->age); // Prints "7"
	printf("%d\n", get_person("john")->age); // Prints "42"
	printf("%d\n", get_person("carl")->age); // Prints "69"
	printf("%p\n", (void *)get_person("bob")); // Prints "(nil)"
}
