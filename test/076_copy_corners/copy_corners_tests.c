#include "../test.h"

void copy_corners_fast(cube_fast_t *, cube_fast_t);
cube_fast_t cubetofast(cube_t);
cube_t fasttocube(cube_fast_t);

int main(void) {
	char str[STRLENMAX];
	cube_t c1, c2;
	cube_fast_t f1, f2;

	fgets(str, STRLENMAX, stdin);
	c1 = readcube("H48", str);
	f1 = cubetofast(c1);

	fgets(str, STRLENMAX, stdin);
	c2 = readcube("H48", str);
	f2 = cubetofast(c2);

	copy_corners_fast(&f1, f2);

	c1 = fasttocube(f1);
	if (iserror(c1)) {
		printf("Error setting EO\n");
	} else if (!isconsistent(c1)) {
		printf("Setting EO resulted in inconsistent cube\n");
	} else {
		writecube("H48", c1, str);
		printf("%s\n", str);
	}

	return 0;
}
