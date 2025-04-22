#include "../test.h"

void copy_corners(cube_t *, cube_t);

void run(void) {
	char str[STRLENMAX];
	cube_t c1, c2;

	fgets(str, STRLENMAX, stdin);
	c1 = readcube(str);

	fgets(str, STRLENMAX, stdin);
	c2 = readcube(str);

	copy_corners(&c1, c2);

	if (iserror(c1)) {
		printf("Error setting EO\n");
	} else if (!isconsistent(c1)) {
		printf("Setting EO resulted in inconsistent cube\n");
	} else {
		writecube(c1, NISSY_SIZE_CUBE, str);
		printf("%s\n", str);
	}
}
