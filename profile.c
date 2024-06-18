// Solely so stupid VS Code can find "CLOCK_PROCESS_CPUTIME_ID"
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PERSONS 123456789
#define ROUNDS 10000

struct person {
	char *name;
	uint32_t age;
};

static char *names[] = {
	"trez",
	"john",
	"carl",
};
#define NAMES_SIZE (sizeof(names) / sizeof(*names))

static struct person persons[MAX_PERSONS];
static size_t persons_size;

static uint32_t buckets[MAX_PERSONS];

static uint32_t chains[MAX_PERSONS];
static size_t chains_size;

static double get_elapsed_seconds(struct timespec start, struct timespec end) {
	return (double)(end.tv_sec - start.tv_sec) + 1.0e-9 * (double)(end.tv_nsec - start.tv_nsec);
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

static struct person *get_person(char *name) {
	uint32_t hash = elf_hash(name);
	uint32_t bucket_index = hash % persons_size;

	uint32_t i = buckets[bucket_index];

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

static void time_hash_table(size_t persons_size) {
	assert(persons_size <= MAX_PERSONS);

	struct timespec start;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (size_t i = 0; i < ROUNDS; i++) {
		// TODO: The persons array needs to have equal distribution
		if (get_person("bob")) {
			printf("Unreachable\n");
		}
	}

	struct timespec end;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	printf("table(%zu) took %.2f seconds\n", persons_size, get_elapsed_seconds(start, end));
}

static void time_array(size_t persons_size) {
	assert(persons_size <= MAX_PERSONS);

	struct timespec start;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (size_t i = 0; i < ROUNDS; i++) {
		size_t goal_index = rand() % persons_size;

		// The actual algorithm being tested
		for (size_t i = 0; i < goal_index; i++) {
			struct person person = persons[i];

			if (strcmp(person.name, "bob") == 0) {
				printf("Unreachable\n");
			}
		}
	}

	struct timespec end;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	printf("array(%zu) took %.2f seconds\n", persons_size, get_elapsed_seconds(start, end));
}

static void push_chain(uint32_t chain) {
	if (chains_size >= MAX_PERSONS) {
		fprintf(stderr, "There are more than %d chains, exceeding MAX_PERSONS\n", MAX_PERSONS);
		exit(EXIT_FAILURE);
	}
	chains[chains_size++] = chain;
}

static void hash_persons(void) {
	memset(buckets, UINT32_MAX, persons_size * sizeof(uint32_t));

	chains_size = 0;

	for (size_t i = 0; i < persons_size; i++) {
		uint32_t hash = elf_hash(persons[i].name);
		uint32_t bucket_index = hash % persons_size;

		push_chain(buckets[bucket_index]);

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
	for (size_t i = 0; i < MAX_PERSONS; i++) {
		push_person(names[rand() % NAMES_SIZE], rand() % UINT32_MAX);
	}
	hash_persons();

	time_array(1);
	time_array(10);
	time_array(100);
	time_array(1000);
	time_array(10000);
	time_array(100000);

	time_hash_table(1);
	time_hash_table(10);
	time_hash_table(100);
	time_hash_table(1000);
	time_hash_table(10000);
	time_hash_table(100000);
	time_hash_table(1000000);
	time_hash_table(10000000);
	time_hash_table(100000000);
}
