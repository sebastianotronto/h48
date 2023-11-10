#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef CUBE_AVX2
#include <immintrin.h>
#endif

#include "../../cube.h"

#define STRLENMAX 10000
#define MOVESMAX 1000

int main() {
	char movestr[STRLENMAX], cubestr[STRLENMAX];
	cube_t cube;

	fgets(movestr, STRLENMAX, stdin);
	fgets(cubestr, STRLENMAX, stdin);
	cube = readcube("H48", cubestr);

	cube = applymoves(cube, movestr);

	if (iserror(cube)) {
		printf("Error moving cube\n");
	} else if (!issolvable(cube)) {
		printf("Moved cube is not solvable\n");
	} else {
		writecube("H48", cube, cubestr);
		printf("%s\n", cubestr);
	}

	return 0;
}
