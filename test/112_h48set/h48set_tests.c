#include "../test.h"

char str[STRLENMAX];

typedef struct {
	int64_t n;
	int64_t capacity;
	int64_t mod;
	int64_t *table;
} h48set_t;

void h48set_create(h48set_t *, int64_t, int64_t);
void h48set_clear(h48set_t *);
void h48set_destroy(h48set_t *);
int64_t h48set_lookup(h48set_t *, int64_t);
void h48set_insert(h48set_t *, int64_t);
bool h48set_contains(h48set_t *, int64_t);
int64_t h48set_save(h48set_t *, int64_t *);

int compare(const void *x, const void *y) {
	int64_t a = *(int64_t *)x;
	int64_t b = *(int64_t *)y;

	if (a > b) return 1;
	if (a == b) return 0;
	return -1;
}

int64_t readl(void) {
	fgets(str, STRLENMAX, stdin);
	return atoll(str);
}

void run(void) {
	h48set_t set;
	int64_t n, i, k, capacity, mod, *a, *b;

	capacity = readl();
	mod = readl();
	n = readl();

	a = malloc(n * sizeof(int64_t));
	b = malloc(n * sizeof(int64_t));
	for (i = 0; i < n; i++)
		a[i] = readl();

	h48set_create(&set, capacity, mod);
	for (i = 0; i < n; i++)
		h48set_insert(&set, a[i]);

	k = h48set_save(&set, b);
	qsort(b, k, sizeof(int64_t), compare);

	printf("%" PRId64 "\n", k);
	for (i = 0; i < k; i++)
		printf("%" PRId64 "\n", b[i]);
	for (i = 0; i < n; i++)
		if (!h48set_contains(&set, a[i]))
			printf("Set does not contain %" PRId64 "\n", a[i]);

	h48set_destroy(&set);
	free(a);
	free(b);
}
