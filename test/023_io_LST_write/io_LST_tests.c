#include "../test.h"

#define STRLENMAX 10000

bool iserror(cube_t);
bool issolvable(cube_t);
cube_t readcube(char *, char *);
void writecube(char *, cube_t, char *);

int main() {
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
		writecube("LST", cube, str);
		printf("%s\n", str);
	}

	return 0;
}
