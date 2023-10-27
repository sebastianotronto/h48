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
	writecube(H48, inverse(cube), str);
	fputs(str, stdout);

	return 0;
}
