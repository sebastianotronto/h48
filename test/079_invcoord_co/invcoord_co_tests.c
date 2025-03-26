#include "../test.h"

int64_t coord_co(cube_t);
cube_t invcoord_co(int64_t);

void run(void) {
	char str[STRLENMAX];
	cube_t cube;
	int64_t coord;

	fgets(str, STRLENMAX, stdin);
	coord = atoll(str);

	cube = invcoord_co(coord);

	if (!isconsistent(cube)) {
		printf("Not consistent\n");
		return;
	}
	if (!issolvable(cube)) {
		printf("Not solvable\n");
		return;
	}

	coord = coord_co(cube);

	printf("%" PRId64 "\n", coord);
}
