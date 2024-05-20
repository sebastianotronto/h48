#include "../test.h"

int64_t coord_fast_esep(cube_fast_t);
cube_fast_t invcoord_fast_esep(int64_t);

int main(void) {
	char str[STRLENMAX];
	cube_t cube;
	cube_fast_t fast;
	int64_t i;

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);
	fast = cubetofast(cube);

	i = coord_fast_esep(fast);
	fast = invcoord_fast_esep(i);
	i = coord_fast_esep(fast);

	printf("%" PRId64 "\n", i);

	return 0;
}
