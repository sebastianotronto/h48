#include "../test.h"

int64_t coord_esep(cube_t);
cube_t invcoord_esep(int64_t);

void run(void) {
	char str[STRLENMAX];
	oriented_cube_t cube;
	int64_t i;

	fgets(str, STRLENMAX, stdin);
	cube = readcube(str);

	i = coord_esep(cube.cube);
	cube.cube = invcoord_esep(i);
	i = coord_esep(cube.cube);

	printf("%" PRId64 "\n", i);
}
