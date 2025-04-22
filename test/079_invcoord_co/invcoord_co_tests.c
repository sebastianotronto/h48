#include "../test.h"

#define POW_3_7 2187

int64_t coord_co(cube_t);
cube_t invcoord_co(int64_t);

void run(void) {
	oriented_cube_t cube;
	int64_t coord, coord2;

	cube.orientation = 0;

	/* Test all possible values for CO coordinate */
	for (coord = 0; coord < POW_3_7; coord++) {
		cube.cube = invcoord_co(coord);

		if (!isconsistent(cube)) {
			printf("Not consistent\n");
			return;
		}
		if (!issolvable(cube)) {
			printf("Not solvable\n");
			return;
		}

		coord2 = coord_co(cube.cube);
		if (coord != coord2) {
			printf("Error: invcoord of %" PRId64
			    " returns %" PRId64 "\n", coord, coord2);
			return;
		}
	}

	printf("All good\n");
}
