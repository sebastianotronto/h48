/*
 * This test is tricky, we only have two cases for now: the solved cube
 * and the cube that is one quarter-turn-move off (all such cases are
 * equivalent due to symmetry). Adding more tests requires figuring out
 * by hand which one is the first position in its class to be reached.
 * Note that the .out file need a space before each newline.
 */
#include "../test.h"

#define COCSEP_CLASSES 3393

size_t gendata_cocsep(void *, uint64_t *, cube_fast_t *);
int64_t coord_fast_cocsep(cube_fast_t);

int main(void) {
	char str[STRLENMAX];
	uint32_t buf[300000], data;
	int64_t coord, coclass;
	uint64_t selfsim[COCSEP_CLASSES], sim, t;
	cube_fast_t fast, rep[COCSEP_CLASSES];

	gendata_cocsep(buf, selfsim, rep);

	/* All cases in the same test so we do not generate data many times */

	while (fgets(str, STRLENMAX, stdin) != NULL) {
		fast = cubetofast(readcube("H48", str));
		coord = coord_fast_cocsep(fast);
		data = buf[coord];
		coclass = (data & (0xFFFU << 16)) >> 16;
		sim = selfsim[coclass];
		for (t = 0; t < 48 && sim; t++, sim >>= 1) {
			if (sim & 1)
				printf("%" PRId64 " ", t);
		}
		printf("\n");
	}

	return 0;
}
