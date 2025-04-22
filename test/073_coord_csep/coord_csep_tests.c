#include "../test.h"

int64_t coord_csep(cube_t);

void run(void) {
	char str[STRLENMAX];
	oriented_cube_t cube;
	int64_t result;

	fgets(str, STRLENMAX, stdin);
	cube = readcube(str);

	result = coord_csep(cube.cube);

	printf("%" PRId64 "\n", result);
}
