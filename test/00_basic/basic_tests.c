#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../src/cube.h"

int main() {
	cube_t z, s;

	s = solvedcube;
	printf("Solved %s consistent\n", isconsistent(s) ? "is" : "is NOT");
	printf("Solved %s solved\n", issolved(s) ? "is" : "is NOT");

	z = (cube_t){0};
	printf("Zero %s consistent\n", isconsistent(z) ? "is" : "is NOT");
	printf("Zero %s solved\n", issolved(z) ? "is" : "is NOT");

	return 0;
}
