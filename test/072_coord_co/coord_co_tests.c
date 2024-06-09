#include "../test.h"

int64_t coord_co(cube_t);

int main(void) {
	char str[STRLENMAX];
	cube_t cube;
	int64_t result;

	fgets(str, STRLENMAX, stdin);
	cube = readcube("H48", str);

	result = coord_co(cube);

	printf("%" PRId64 "\n", result);

	return 0;
}
