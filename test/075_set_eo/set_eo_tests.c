#include "../test.h"

int64_t coord_eo(cube_t);
void set_eo(cube_t *, int64_t);
void pieces(cube_t *, uint8_t [static 8], uint8_t [static 12]);

void run(void) {
	char str[STRLENMAX];
	cube_t cube;
	uint8_t edge[12], corner[8];
	int64_t eo;

	fgets(str, STRLENMAX, stdin);
	cube = readcube(str);
	fgets(str, STRLENMAX, stdin);
	eo = atoi(str);

	set_eo(&cube, eo);

	if (iserror(cube)) {
		printf("Error setting EO\n");
	} else if (!isconsistent(cube)) {
		pieces(&cube, corner, edge);
		fprintf(stderr, "edges: ");
		for (int i = 0; i < 12; i++)
			fprintf(stderr, "%d ", edge[i]);
		fprintf(stderr, "\n");
		for (int i = 0; i < 8; i++)
			fprintf(stderr, "%d ", corner[i]);
		fprintf(stderr, "\n");
		printf("Setting EO resulted in inconsistent cube\n");
	} else {
		writecube(cube, NISSY_SIZE_CUBE, str);
		printf("%s\n", str);
	}
}
