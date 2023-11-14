#include "../test.h"

cube_fast_t cubetofast(cube_t);
cube_t fasttocube(cube_fast_t);
bool equal(cube_t, cube_t);

int main() {
	char cubestr[STRLENMAX];
	cube_t cube, cube2;
	cube_fast_t fast;

	fgets(cubestr, STRLENMAX, stdin);
	cube = readcube("H48", cubestr);
	fast = cubetofast(cube);
	cube2 = fasttocube(fast);

	if (iserror(cube)) {
		printf("Error reading cube\n");
	} else if (iserror(cube2)) {
		printf("Error converting cube\n");
	} else {
		writecube("H48", cube2, cubestr);
		printf("%s\n", cubestr);
	}

	return 0;
}
