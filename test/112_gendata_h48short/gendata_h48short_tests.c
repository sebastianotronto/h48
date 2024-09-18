#include "../test.h"

#define COCSEP_CLASSES 3393
#define INFOSIZE       512
#define MAXPOS         200

typedef struct {
	uint64_t n;
	uint64_t capacity;
	uint64_t randomizer;
	uint64_t *table;
	uint32_t *info;
	uint32_t *cocsepdata;
	uint32_t *h48data;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t crep[COCSEP_CLASSES];
} h48map_t;

typedef struct {
	uint64_t key;
	uint64_t val;
} kvpair_t;

typedef struct {
	uint8_t maxdepth;
	const uint32_t *cocsepdata;
	const cube_t *crep;
	const uint64_t *selfsim;
	h48map_t *map;
} gendata_h48short_arg_t;

void h48map_create(h48map_t *, uint64_t, uint64_t);
void h48map_destroy(h48map_t *);
kvpair_t h48map_nextkvpair(h48map_t *, uint64_t *);
size_t gendata_cocsep(void *, uint64_t *, cube_t *);
uint64_t gendata_h48short(gendata_h48short_arg_t *);

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
	char buf[2000000];
	h48map_t map;
	uint64_t i, j, capacity, randomizer, selfsim[COCSEP_CLASSES];
	kvpair_t kv, b[MAXPOS];
	gendata_h48short_arg_t arg;
	cube_t crep[COCSEP_CLASSES];

	capacity = readl();
	randomizer = readl();
	arg.maxdepth = readl();
	arg.crep = crep;
	arg.selfsim = selfsim;
	arg.map = &map;

	h48map_create(&map, capacity, randomizer);
	gendata_cocsep(buf, selfsim, crep);
	arg.cocsepdata = (uint32_t *)((char *)buf + INFOSIZE);
	gendata_h48short(&arg);

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
