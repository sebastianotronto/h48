#include "../test.h"

int main(void) {
	char str[STRLENMAX];
	cube_t cube;

	fgets(str, STRLENMAX, stdin);

	cube = readcube("B32", str);

	if (iserror(cube)) {
		printf("Error reading cube\n");
	} else if (!issolvable(cube)) {
		printf("Cube is not solvable\n");
	} else {
		writecube("B32", cube, str);
		printf("%s\n", str);
	}

	return 0;
}
