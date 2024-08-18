#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "../src/nissy.h"

#define STRLENMAX 1000

int main(void) {
	char str[STRLENMAX], cube[22], inv[22];

	fgets(str, STRLENMAX, stdin);
	nissy_readcube("H48", str, cube);
	nissy_inverse(cube, inv);
	nissy_writecube("H48", inv, str);
	fputs(str, stdout);

	return 0;
}
