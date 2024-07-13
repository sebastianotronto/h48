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
	bool f;
	h48set_t set;
	int64_t n, i, j, capacity, mod, *a, u;

	capacity = readl();
	mod = readl();
	n = readl();

	a = malloc(n * sizeof(int64_t));
	for (i = 0; i < n; i++)
		a[i] = readl();

	/* Count unique elements */
	u = 0;
	for (i = 0; i < n; i++) {
		for (j = 0, f = true; j < i; j++)
			f = f && a[i] != a[j];
		u += f;
	}

	h48set_create(&set, capacity, mod);
	for (i = 0; i < n; i++)
		h48set_insert(&set, a[i]);

	for (i = 0, j = 0; i < set.capacity; i++)
		if (set.table[i] != -1)
			a[j++] = set.table[i];
	qsort(a, j, sizeof(int64_t), compare);

	printf("%" PRId64 "\n", set.n);
	for (i = 0; i < j; i++)
		printf("%" PRId64 "\n", a[i]);

	h48set_destroy(&set);
	free(a);
}
