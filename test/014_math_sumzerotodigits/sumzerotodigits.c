#include "../test.h"

void sumzerotodigits(int64_t, uint8_t, uint8_t, uint8_t *);

void run(void) {
	char str[STRLENMAX];
	uint8_t i, n, b, a[100];
	int64_t d;

	fgets(str, STRLENMAX, stdin);
	n = atoi(str);
	fgets(str, STRLENMAX, stdin);
	b = atoi(str);
	fgets(str, STRLENMAX, stdin);
	d = atoll(str);

	sumzerotodigits(d, n, b, a);
	
	for (i = 0; i < n; i++)
		printf("%" PRIu8 "\n", a[i]);
}
