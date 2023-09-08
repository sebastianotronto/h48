#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../src/cube.h"

#define STRLENMAX 10000

int main() {
	char *c, str[STRLENMAX];
	cube_t cube;

	for (c = str; (*c = getchar()) != EOF; c++) ;
	*c = '\0';

	cube = readcube(str);

	if (iserror(cube)) {
		printf("Error reading cube\n");
	} else if (!isconsistent(cube)) {
		printf("Cube is inconsistent\n");
	} else {
		writecube(cube, str);
		printf("%s\n", str);
	}

	return 0;
}
