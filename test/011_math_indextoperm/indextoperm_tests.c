#include "../test.h"

void indextoperm(int64_t, size_t n, uint8_t [n]);

void run(void) {
	char str[STRLENMAX];
	uint8_t a[100];
	int64_t n, p, i;

	fgets(str, STRLENMAX, stdin);
	n = atoll(str);
	fgets(str, STRLENMAX, stdin);
	p = atoll(str);

	indextoperm(p, n, a);

	for (i = 0; i < n; i++)
		printf("%" PRIu8 "\n", a[i]);
}
