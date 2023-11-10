#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef CUBE_AVX2
#include <immintrin.h>
#endif

#include "../../cube.h"

#define STRLENMAX 10000

int main() {
	char cubestr[STRLENMAX], transtr[STRLENMAX];
	cube_t cube;

	fgets(transtr, STRLENMAX, stdin);
	fgets(cubestr, STRLENMAX, stdin);
	cube = readcube("H48", cubestr);

	cube = applytrans(cube, transtr);

	if (iserror(cube)) {
		printf("Error transforming cube\n");
	} else if (!issolvable(cube)) {
		printf("Transformed cube is not solvable\n");
	} else {
		writecube("H48", cube, cubestr);
		printf("%s\n", cubestr);
	}

	return 0;
}
