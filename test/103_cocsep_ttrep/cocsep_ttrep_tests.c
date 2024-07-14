#include "../test.h"

#define COCSEP_CLASSES 3393
#define TTREP_MASK            (UINT32_C(0xFF) << UINT32_C(8))
#define TTREP(x)              (((x) & TTREP_MASK) >> UINT32_C(8))

uint8_t inverse_trans(uint8_t);
cube_t transform_corners(cube_t, uint8_t);
int64_t coord_cocsep(cube_t);
size_t gendata_cocsep(void *, uint64_t *, cube_t *);

void run(void) {
	uint8_t t;
	uint32_t buf[300000], tt;
	uint64_t selfsim[COCSEP_CLASSES];
	int64_t i, j, k, l;
	cube_t rep[COCSEP_CLASSES], c, d;

	gendata_cocsep(buf, selfsim, rep);

	for (i = 0; i < COCSEP_CLASSES; i++) {
		c = rep[i];
		for (t = 0; t < 48; t++) {
			d = transform_corners(c, t);
			j = coord_cocsep(d);
			tt = TTREP(buf[j]);
			d = transform_corners(d, tt);
			k = coord_cocsep(d);
			l = coord_cocsep(c);
			if (k != l)
				printf("cocsep %" PRId64 " (%" PRId64 "): "
				    "%" PRId64 " ttrep %" PRIu8
				    " ->  %" PRId64 "\n", i, l, j, tt, k);
		}
	}
}
