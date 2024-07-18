#include "../test.h"

#define COCSEP_CLASSES 3393
#define MAXPOS 200

typedef struct {
	uint64_t n;
	uint64_t capacity;
	uint64_t randomizer;
	uint64_t *table;
} h48map_t;

typedef struct {
	uint64_t key;
	uint64_t val;
} kvpair_t;

void h48map_create(h48map_t *, uint64_t, uint64_t);
void h48map_destroy(h48map_t *);
kvpair_t h48map_nextkvpair(h48map_t *, uint64_t *);
size_t gendata_cocsep(void *, uint64_t *, cube_t *);
uint64_t gen_h48short(
    uint8_t, const uint32_t *, const cube_t *, const uint64_t *, h48map_t *);

char str[STRLENMAX];

int compare(const void *x, const void *y) {
	uint64_t a = ((kvpair_t *)x)->key;
	uint64_t b = ((kvpair_t *)y)->key;

	if (a > b) return 1;
	if (a == b) return 0;
	return -1;
}

uint64_t readl(void) {
	fgets(str, STRLENMAX, stdin);
	return atoll(str);
}

void run(void) {
	uint32_t cocsepdata[300000];
	h48map_t map;
	uint64_t n, i, j, capacity, randomizer, selfsim[COCSEP_CLASSES];
	kvpair_t kv, b[MAXPOS];
	cube_t crep[COCSEP_CLASSES];

	capacity = readl();
	randomizer = readl();
	n = readl();

	h48map_create(&map, capacity, randomizer);
	gendata_cocsep(cocsepdata, selfsim, crep);
	gen_h48short(n, cocsepdata, crep, selfsim, &map);

	i = 0;
	for (kv = h48map_nextkvpair(&map, &i), j = 0;
	     i != map.capacity && j < MAXPOS;
	     kv = h48map_nextkvpair(&map, &i)
	) {
		b[j++] = kv;
	}
	qsort(b, j, sizeof(kvpair_t), compare);

	printf("%" PRIu64 "\n", map.n);
	for (i = 0; i < j; i++)
		printf("%" PRIu64 " %" PRIu64 "\n", b[i].key, b[i].val);

	h48map_destroy(&map);
}
