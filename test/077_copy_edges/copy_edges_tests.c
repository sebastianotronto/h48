#include "../test.h"

void copy_edges(cube_t *, cube_t);

void run(void) {
	char str[STRLENMAX];
	cube_t c1, c2;

	fgets(str, STRLENMAX, stdin);
	c1 = readcube("H48", str);

	fgets(str, STRLENMAX, stdin);
	c2 = readcube("H48", str);

	copy_edges(&c1, c2);

	if (iserror(c1)) {
		printf("Error setting EO\n");
	} else if (!isconsistent(c1)) {
		printf("Setting EO resulted in inconsistent cube\n");
	} else {
		writecube("H48", c1, NISSY_SIZE_H48, str);
		printf("%s\n", str);
	}
}
