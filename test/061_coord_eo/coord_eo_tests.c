#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef CUBE_AVX2
#include <immintrin.h>
#endif

#include "../test.h"

#define STRLENMAX 10000

int64_t coord_fast_eo(cube_fast_t);
cube_fast_t cubetofast(cube_t);

int main() {
	char str[STRLENMAX];
	cube_t cube;
	cube_fast_t fast;
	int16_t result;

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);
	fast = cubetofast(cube);

	result = coord_fast_eo(fast);

	printf("%" PRId16 "\n", result);

	return 0;
}
