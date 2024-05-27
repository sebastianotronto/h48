#include "../test.h"

int64_t coord_fast_eo(cube_fast_t);
void set_eo_fast(cube_fast_t *, int64_t);

int main(void) {
	char str[STRLENMAX];
	cube_t cube;
	cube_fast_t fast;
	int64_t eo;

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);
	fast = cubetofast(cube);
	fgets(str, STRLENMAX, stdin);
	eo = atoi(str);

	set_eo_fast(&fast, eo);

	cube = fasttocube(fast);
	if (iserror(cube)) {
		printf("Error setting EO\n");
	} else if (!isconsistent(cube)) {
		fprintf(stderr, "edges: ");
		for (int i = 0; i < 12; i++)
			fprintf(stderr, "%d ", cube.edge[i]);
		fprintf(stderr, "\n");
		for (int i = 0; i < 8; i++)
			fprintf(stderr, "%d ", cube.corner[i]);
		fprintf(stderr, "\n");
		printf("Setting EO resulted in inconsistent cube\n");
	} else {
		writecube("H48", cube, str);
		printf("%s\n", str);
	}

	return 0;
}
