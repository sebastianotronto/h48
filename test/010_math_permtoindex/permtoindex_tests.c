#include "../test.h"

int64_t permtoindex(uint8_t *, int64_t);

void run(void) {
	char str[STRLENMAX];
	uint8_t a[100];
	int64_t n, i, p;

	fgets(str, STRLENMAX, stdin);
	n = atoll(str);
	for (i = 0; i < n; i++) {
		fgets(str, STRLENMAX, stdin);
		a[i] = atoi(str);
	}

	p = permtoindex(a, n);
	printf("%" PRId64 "\n", p);
}
