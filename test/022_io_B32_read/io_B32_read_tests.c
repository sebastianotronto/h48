#include "../test.h"

int main(void) {
	char str[STRLENMAX], *aux;
	cube_t cube;

        aux = str;
	while (fgets(aux, STRLENMAX, stdin) != NULL)
		while (*aux != '\n')
			aux++;

	cube = readcube("B32", str);

	if (iserror(cube)) {
		printf("Error reading cube\n");
	} else if (!issolvable(cube)) {
		printf("Cube is not solvable\n");
	} else {
		writecube("H48", cube, str);
		printf("%s\n", str);
	}

	return 0;
}
