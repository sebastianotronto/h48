#include "../test.h"

#define POW_3_7 2187
#define BOUND   45 * POW_3_7
#define TGROUP  UINT64_C(4278190335)

cube_t transform(cube_t, uint8_t);
uint64_t coordinate_dreo_coord(cube_t, const unsigned char *);
cube_t coordinate_dreo_cube(uint64_t, const unsigned char *);
uint64_t coordinate_dreo_gendata(unsigned char *);

void run(void) {
	bool found;
	uint64_t t;
	char str[STRLENMAX];
	unsigned char *data;
	size_t size;
	cube_t cube;
	uint64_t coord, coord2;

	size = coordinate_dreo_gendata(NULL);
	data = malloc(size);
	coordinate_dreo_gendata(data);

	for (coord = 0; coord < BOUND; coord++) {
		cube = coordinate_dreo_cube(coord, data);

		if (!isconsistent(cube)) {
			printf("Error: invcoord of %" PRId64
			    " is not consistent\n", coord);
			goto cleanup;
		}

		for (t = 0, found = false; t < 48; t++) {
			if (!((UINT64_C(1) << t) & TGROUP))
				continue;

			coord2 = coordinate_dreo_coord(transform(cube, t), data);
			if (coord == coord2) {
				found = true;
				break;
			}
		}

		if (!found) {
			printf("Error: invcoord of %" PRId64 " returns %"
			    PRId64 " with cube:\n", coord, coord2);
			writecube(cube, STRLENMAX, str);
			printf("%s\n", str);
			goto cleanup;
		}
	}

	printf("All good\n");

cleanup:
	free(data);
}
