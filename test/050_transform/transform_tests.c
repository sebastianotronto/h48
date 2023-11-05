#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../cube.h"

#define STRLENMAX 10000

int main() {
	char str[STRLENMAX];
	trans_t t;
	cube_t cube;

	fgets(str, STRLENMAX, stdin);
	t = readtrans(str);

	if (t >= 48) {
		printf("Error reading trans\n");
		return 1;
	}

	fgets(str, STRLENMAX, stdin);
	cube = readcube(H48, str);

	cube = transform(cube, t);

	if (iserror(cube)) {
		printf("Error transforming cube\n");
	} else if (!issolvable(cube)) {
		printf("Transformed cube is not solvable\n");
	} else {
		writecube(H48, cube, str);
		printf("%s\n", str);
	}

	return 0;
}
