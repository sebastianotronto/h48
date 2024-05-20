#include "../test.h"

int64_t coord_fast_co(cube_fast_t);

int main(void) {
	char str[STRLENMAX];
	cube_t cube;
	cube_fast_t fast;
	int64_t result;

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);
	fast = cubetofast(cube);

	result = coord_fast_co(fast);

	printf("%" PRId64 "\n", result);

	return 0;
}
