#include "../test.h"

int main(void) {
	char *c, str[STRLENMAX];
	cube_t cube;

	for (c = str; (*c = getchar()) != EOF; c++) ;
	*c = '\0';

	cube = readcube("H48", str);

	if (iserror(cube)) {
		printf("Error reading cube\n");
	} else if (!issolvable(cube)) {
		printf("Cube is not solvable\n");
	} else {
		writecube("SRC", cube, str);
		printf("%s\n", str);
	}

	return 0;
}
