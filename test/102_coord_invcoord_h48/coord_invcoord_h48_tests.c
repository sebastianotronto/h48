#include "../test.h"

#define COCSEP_CLASSES 3393

size_t gendata_cocsep(void *, uint64_t *, cube_fast_t *);
int64_t coord_h48(cube_fast_t, const uint32_t *, uint8_t);
cube_fast_t invcoord_h48(int64_t, const cube_fast_t *, uint8_t);
cube_fast_t transform(cube_fast_t, uint8_t);

int main(void) {
	char str[STRLENMAX];
	int i;
	bool found;
	uint8_t h, t;
	uint32_t cocsepdata[300000];
	uint64_t selfsim[COCSEP_CLASSES];
	int64_t c, cc;
	cube_t cube;
	cube_fast_t fast, invc, rep[COCSEP_CLASSES];

	gendata_cocsep(cocsepdata, selfsim, rep);

	i = 1;
	h = 11;
	while (fgets(str, STRLENMAX, stdin) != NULL) {
		cube = readcube("H48", str);
		fast = cubetofast(cube);
		c = coord_h48(fast, cocsepdata, h);
		invc = invcoord_h48(c, rep, h);
		for (t = 0, found = false; t < 48; t++) {
			fast = transform(invc, t);
			cc = coord_h48(fast, cocsepdata, h);
			found = found || cc == c;
		}
		printf("%d %s\n", i, found ? "ok" : "ERROR");
		i++;
	}

	return 0;
}
