#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "../src/nissy.h"

#define STRLENMAX 1000

int main(void) {
	char strin[STRLENMAX], strout[STRLENMAX];
	int result;

	fgets(strin, STRLENMAX, stdin);

	result = nissy_convertcube("H48", "LST", strin, strout);
	if (result)
		fprintf(stderr, "Error converting cube: code %d\n", result);

	fputs(strout, stdout);

	return 0;
}
