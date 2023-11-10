#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef CUBE_AVX2
#include <immintrin.h>
#endif

#include "../../cube.h"

#define STRLENMAX 10000

int main() {
	char str[STRLENMAX];
	cube_t cube, inv;

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);
	inv = inverse(cube);

	if (iserror(inv)) {
		printf("Error inverting cube\n");
	} else if (!issolvable(inv)) {
		printf("Inverted cube is not solvable\n");
	} else {
		writecube("H48", inv, str);
		printf("%s\n", str);
	}

	return 0;
}
