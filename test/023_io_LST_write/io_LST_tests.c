#include "../test.h"

#define STRLENMAX 10000

bool iserror(cube_t);
bool issolvable(cube_t);
cube_t readcube(char *, char *);
void writecube(char *, cube_t, char *);

int main(void) {
	char str[STRLENMAX], *aux;
	cube_t cube;

        aux = str;
	while (fgets(aux, STRLENMAX, stdin) != NULL)
		while (*aux != '\n')
			aux++;

	cube = readcube("H48", str);

	if (iserror(cube)) {
		printf("Error reading cube\n");
	} else if (!issolvable(cube)) {
		printf("Cube is not solvable\n");
	} else {
		writecube("LST", cube, str);
		printf("%s\n", str);
	}

	return 0;
}
