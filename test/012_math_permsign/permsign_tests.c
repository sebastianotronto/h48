#include "../test.h"

int permsign(uint8_t *, int64_t);

void run(void) {
	char str[STRLENMAX];
	uint8_t a[100];
	int p;
	int64_t n, i;

	fgets(str, STRLENMAX, stdin);
	n = atoll(str);

	for (i = 0; i < n; i++) {
		fgets(str, STRLENMAX, stdin);
		a[i] = atoi(str);
	}

	p = permsign(a, n);
	printf("%d\n", p);
}
