#include "../test.h"

int64_t digitstosumzero(uint8_t *, uint8_t, uint8_t);

void run(void) {
	char str[STRLENMAX];
	uint8_t i, n, b, a[100];
	int64_t p;

	fgets(str, STRLENMAX, stdin);
	n = atoi(str);
	fgets(str, STRLENMAX, stdin);
	b = atoi(str);
	for (i = 0; i < n; i++) {
		fgets(str, STRLENMAX, stdin);
		a[i] = atoi(str);
	}

	p = digitstosumzero(a, n, b);
	printf("%" PRId64 "\n", p);
}
