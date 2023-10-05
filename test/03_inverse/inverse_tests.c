#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../src/cube.h"

#define STRLENMAX 10000

int main() {
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
		writecube(inv, str);
		printf("%s\n", str);
	}

	return 0;
}
