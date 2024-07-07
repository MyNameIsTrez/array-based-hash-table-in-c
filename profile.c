// Solely so stupid VS Code can find "CLOCK_PROCESS_CPUTIME_ID"
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PERSONS 12345678
#define ROUNDS 10000

struct person {
	char *name;
	uint32_t age;
};

// The name for person 123 will be "123", where the +1 is for the '\0'
static char names[MAX_PERSONS][8 + 1];

static struct person persons[MAX_PERSONS];
static size_t persons_size;

static uint32_t buckets[MAX_PERSONS];
static uint32_t chains[MAX_PERSONS];

static double get_elapsed_seconds(struct timespec start, struct timespec end) {
	return (double)(end.tv_sec - start.tv_sec) + 1.0e-9 * (double)(end.tv_nsec - start.tv_nsec);
}

static void time_array(size_t limit) {
	size_t hits = 0;

	struct timespec start;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (size_t i = 0; i < ROUNDS; i++) {
		char *goal_name = names[rand() % limit];

		// The actual algorithm that we want to time
		for (size_t i = 0; i < limit; i++) {
			struct person person = persons[i];

			if (strcmp(person.name, goal_name) == 0) {
				hits++;
				break;
			}
		}
	}

	struct timespec end;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	printf("time_array(%zu) had %zu/%d hits, taking %.2f seconds\n", limit, hits, ROUNDS, get_elapsed_seconds(start, end));
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

static void time_hash_table(void) {
	size_t hits = 0;

	struct timespec start;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (size_t i = 0; i < ROUNDS; i++) {
		// The actual algorithm that we want to time
		if (get_person(names[rand() % MAX_PERSONS])) {
			hits++;
		}
	}

	struct timespec end;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	printf("time_hash_table() had %zu/%d hits, taking %.2f seconds\n", hits, ROUNDS, get_elapsed_seconds(start, end));
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

static size_t pow10_size(size_t count) {
	size_t result = 1;
	for (size_t i = 0; i < count; i++) {
		result *= 10;
	}
	return result;
}

int main(void) {
	srand(time(NULL));

	printf("MAX_PERSONS is %d\n", MAX_PERSONS);
	printf("ROUNDS is %d\n\n", ROUNDS);

	// Makes sure names[42] will be "00000042"
	for (size_t p = 0; p < MAX_PERSONS; p++) {
		for (size_t c = 0; c < 8; c++) {
			names[p][7-c] = '0' + (p / pow10_size(c)) % 10;
		}
		names[p][8] = '\0';
	}
	printf("names[42] is %s\n", names[42]);
	printf("names[MAX_PERSONS-1] is %s\n\n", names[MAX_PERSONS-1]);

	for (size_t i = 0; i < MAX_PERSONS; i++) {
		push_person(names[rand() % MAX_PERSONS], rand() % UINT32_MAX);
	}
	printf("persons[42].name is %s\n", persons[42].name);
	printf("persons[MAX_PERSONS-1].name is %s\n\n", persons[MAX_PERSONS-1].name);

	hash_persons();

	time_hash_table();
	printf("\n");

	time_array(1);
	time_array(10);
	time_array(100);
	time_array(1000);
	time_array(10000);
	time_array(100000);
}
