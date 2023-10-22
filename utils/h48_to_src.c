#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "../src/cube.h"

#define STRLENMAX 1000

int main() {
	char str[STRLENMAX];
	cube_t cube;

	fgets(str, STRLENMAX, stdin);
	cube = readcube(H48, str);
	writecube(SRC, cube, str);
	fputs(str, stdout);
	/* putc('\n', stdout); */

	return 0;
}
