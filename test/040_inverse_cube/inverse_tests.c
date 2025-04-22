#include "../test.h"

cube_t inverse(cube_t);

void run(void) {
	char str[STRLENMAX];
	cube_t cube, inv;

	fgets(str, STRLENMAX, stdin);
	cube = readcube(str);
	inv = inverse(cube);

	if (iserror(inv)) {
		printf("Error inverting cube\n");
	} else if (!issolvable(inv)) {
		printf("Inverted cube is not solvable\n");
	} else {
		writecube(inv, NISSY_SIZE_CUBE, str);
		printf("%s\n", str);
	}
}
