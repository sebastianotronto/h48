#include "../test.h"

size_t gendata_cocsep(unsigned char *, uint64_t *, cube_t *);
cube_t transform(cube_t, uint8_t);
int64_t coord_cocsep(cube_t);

void run(void) {
	uint8_t t;
	unsigned char buf[2000000];
	uint32_t *cocsepdata;
	uint64_t selfsim[COCSEP_CLASSES];
	int64_t coord, tcoord;
	char str[STRLENMAX];
	cube_t cube, transd, rep[COCSEP_CLASSES];

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);

	gendata_cocsep(buf, selfsim, rep);
	cocsepdata = (uint32_t *)((char *)buf + INFOSIZE);

	coord = (int64_t)COCLASS(cocsepdata[coord_cocsep(cube)]);
	for (t = 0; t < 48; t++) {
		transd = transform(cube, t);
		tcoord = (int64_t)COCLASS(cocsepdata[coord_cocsep(transd)]);
		if (coord != tcoord)
			printf("Error: expected %" PRId64
			    " but got %" PRId64 "\n", coord, tcoord);
	}
}
