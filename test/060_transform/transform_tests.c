#include "../test.h"

cube_t applytrans(cube_t, char *);

void run(void) {
	char cubestr[STRLENMAX], transtr[STRLENMAX];
	cube_t cube;

	fgets(transtr, STRLENMAX, stdin);
	fgets(cubestr, STRLENMAX, stdin);
	cube = readcube("H48", cubestr);

	cube = applytrans(cube, transtr);

	if (iserror(cube)) {
		printf("Error transforming cube\n");
	} else if (!issolvable(cube)) {
		printf("Transformed cube is not solvable\n");
	} else {
		writecube("H48", cube, NISSY_SIZE_H48, cubestr);
		printf("%s\n", cubestr);
	}
}
