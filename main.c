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
static size_t chains_size;

static struct person *get_person(char *name) {
	(void)name;
	return &(struct person){0};
}

// From https://sourceware.org/git/?p=binutils-gdb.git;a=blob;f=bfd/elf.c#l193
static uint32_t elf_hash(const char *namearg) {
	uint32_t h = 0;
	for (const unsigned char *name = (const unsigned char *) namearg; *name; name++) {
		h = (h << 4) + *name;
		h ^= (h >> 24) & 0xf0;
	}
	return h & 0x0fffffff;
}

static void push_chain(uint32_t chain) {
	if (chains_size >= MAX_PERSONS) {
		fprintf(stderr, "There are more than %d symbols, exceeding MAX_PERSONS", MAX_PERSONS);
		exit(EXIT_FAILURE);
	}
	chains[chains_size++] = chain;
}

static void hash_persons(void) {
	uint32_t nbucket = persons_size;

	memset(buckets, 0, nbucket * sizeof(uint32_t));

	chains_size = 0;

	push_chain(0); // The first entry in the chain is always STN_UNDEF

	for (size_t i = 0; i < persons_size; i++) {
		uint32_t hash = elf_hash(persons[i].name);
		uint32_t bucket_index = hash % nbucket;

		push_chain(buckets[bucket_index]);

		buckets[bucket_index] = i + 1;
	}

	// for (size_t i = 0; i < nbucket; i++) {
	// 	push_number(buckets[i], 4);
	// }

	// for (size_t i = 0; i < chains_size; i++) {
	// 	push_number(chains[i], 4);
	// }
}

static void push_person(char *name, uint32_t age) {
	if (persons_size >= MAX_PERSONS) {
		fprintf(stderr, "There are more than %d persons, exceeding MAX_PERSONS", MAX_PERSONS);
		exit(EXIT_FAILURE);
	}
	persons[persons_size++] = (struct person){ .name = name, .age = age };
}

int main(void) {
	push_person("trez", 7);
	push_person("john", 42);

	hash_persons();

	printf("%d\n", get_person("trez")->age);
	printf("%d\n", get_person("john")->age);
	printf("%p\n", (void *)get_person("bob"));
}
