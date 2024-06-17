#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PERSONS 420420
#define BUCKET_COUNT 420

struct person {
	char *name;
	uint32_t age;
};

static struct person persons[MAX_PERSONS];
static size_t persons_size;

static uint32_t buckets[BUCKET_COUNT];

#define MAX_CHAINS (MAX_PERSONS + 1) // +1, because [0] is a sentinel value
static uint32_t chains[MAX_CHAINS];
static size_t chains_size;

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
	uint32_t hash = elf_hash(name);
	uint32_t bucket_index = hash % BUCKET_COUNT;

	uint32_t ix = buckets[bucket_index];

	while (1) {
		if (ix == 0) {
			return NULL;
		}

		if (strcmp(name, persons[ix - 1].name) == 0) {
			break;
		}

		ix = chains[ix];
	}

	return persons + ix - 1;
}

static void push_chain(uint32_t chain) {
	if (chains_size >= MAX_CHAINS) {
		fprintf(stderr, "There are more than %d chains, exceeding MAX_CHAINS\n", MAX_CHAINS);
		exit(EXIT_FAILURE);
	}
	chains[chains_size++] = chain;
}

static void hash_persons(void) {
	memset(buckets, 0, BUCKET_COUNT * sizeof(uint32_t));

	chains_size = 0;

	push_chain(0); // Sentinel value

	for (size_t i = 0; i < persons_size; i++) {
		uint32_t hash = elf_hash(persons[i].name);
		uint32_t bucket_index = hash % BUCKET_COUNT;

		push_chain(buckets[bucket_index]);

		buckets[bucket_index] = i + 1;
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

	printf("%d\n", get_person("trez")->age);
	printf("%d\n", get_person("john")->age);
	printf("%d\n", get_person("carl")->age);
	printf("%p\n", (void *)get_person("bob"));
}
