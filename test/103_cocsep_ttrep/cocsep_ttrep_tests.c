#include "../test.h"

#define COCSEP_CLASSES 3393

uint8_t inverse_trans(uint8_t);
cube_t transform_corners(cube_t);
int64_t coord_cocsep(cube_t);
size_t gendata_cocsep(void *, uint64_t *, cube_t *);

void run(void) {
	uint8_t t, tinv;
	uint32_t buf[300000], tt;
	uint64_t selfsim[COCSEP_CLASSES];
	int64_t i, j;
	cube_t rep[COCSEP_CLASSES], c, d;

	gendata_cocsep(buf, selfsim, rep);

	for (i = 0; i < COCSEP_CLASSES; i++) {
		c = rep[i];
		for (t = 0; t < 48; t++) {
			tinv = inverse_trans(t);
			d = transform_corners(c);
			j = coord_cocsep(d);
			tt = (buf[j] & (0xFF << 8)) >> 8;
			if (tt != tinv)
				printf("cocsep %" PRId64 " <- %" PRId64 ": "
				    "expected t %" PRIu8 " (inverse of %"
				    PRIu8 "), got %" PRIu32 "\n",
				    i, j, tinv, t, tt);
		}
	}
}
