#include "../test.h"

#define COCLASS_MASK          (UINT32_C(0xFFFF) << UINT32_C(16))
#define COCLASS(x)            (((x) & COCLASS_MASK) >> UINT32_C(16))

#define COCSEP_CLASSES 3393

size_t gendata_cocsep(void *, uint64_t *, cube_t *);
cube_t transform(cube_t, uint8_t);
int64_t coord_cocsep(cube_t);

void run(void) {
	uint8_t t;
	uint32_t buf[300000];
	uint64_t selfsim[COCSEP_CLASSES];
	int64_t coord, tcoord;
	char str[STRLENMAX];
	cube_t cube, transd, rep[COCSEP_CLASSES];

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);

	gendata_cocsep(buf, selfsim, rep);

	coord = (int64_t)COCLASS(buf[coord_cocsep(cube)]);
	for (t = 0; t < 48; t++) {
		transd = transform(cube, t);
		tcoord = (int64_t)COCLASS(buf[coord_cocsep(transd)]);
		if (coord != tcoord)
			printf("Error: expected %" PRId64
			    " but got %" PRId64 "\n", coord, tcoord);
	}
}
