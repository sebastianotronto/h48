#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

#include "../../cube.h"

#define STRLENMAX 10000

int main() {
	char str[STRLENMAX];
	cube_t cube;
	int16_t result;

	fgets(str, STRLENMAX, stdin);
	cube = readcube(H48, str);

	result = coord_eo(cube);

	printf("%" PRId16 "\n", result);

	return 0;
}
