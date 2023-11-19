#include "../test.h"

int permsign(uint8_t *, int);

int main(void) {
	char str[STRLENMAX];
	uint8_t a[100];
	int n, i, p;

	fgets(str, STRLENMAX, stdin);
	n = atoi(str);

	for (i = 0; i < n; i++) {
		fgets(str, STRLENMAX, stdin);
		a[i] = atoi(str);
	}

	p = permsign(a, n);
	printf("%d\n", p);

	return 0;
}
