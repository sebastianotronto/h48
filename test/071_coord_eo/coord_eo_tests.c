#include "../test.h"

int64_t coord_fast_eo(cube_fast_t);
cube_fast_t cubetofast(cube_t);

int main(void) {
	char str[STRLENMAX];
	cube_t cube;
	cube_fast_t fast;
	int64_t result;

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);
	fast = cubetofast(cube);

	result = coord_fast_eo(fast);

	printf("%" PRId64 "\n", result);

	return 0;
}
