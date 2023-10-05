#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../src/cube.h"

#define STRLENMAX 10000

int main() {
	char str[STRLENMAX];
	cube_t c1, c2, c3;

	fgets(str, STRLENMAX, stdin);
	c1 = readcube(str);
	fgets(str, STRLENMAX, stdin);
	c2 = readcube(str);

	c3 = compose(c1, c2);

	if (iserror(c3)) {
		printf("Error composing cubes\n");
	} else if (!issolvable(c3)) {
		printf("Composed cube is not solvable\n");
	} else {
		writecube(c3, str);
		printf("%s\n", str);
	}

	return 0;
}
