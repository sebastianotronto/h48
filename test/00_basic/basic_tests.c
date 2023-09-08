#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../src/cube.h"

void
check(cube_t cube, char *name)
{
	printf("%s is%s consistent\n", name, isconsistent(cube) ? "" : " NOT");
	printf("%s is%s solved\n", name, issolved(cube) ? "" : " NOT");
}

void
check2(cube_t cube1, char *name1, cube_t cube2, char *name2)
{
	printf("%s and %s are%s equal\n", name1, name2,
	    equal(cube1, cube2) ? "" : " NOT");
}

int main() {
	cube_t zero = (cube_t){0};

	check(solvedcube, "Solved");
	check(zero, "Zero");

	check2(solvedcube, "Solved", solvedcube, "Solved");
	check2(solvedcube, "Solved", zero, "Zero");
	check2(zero, "Zero", solvedcube, "Solved");

	return 0;
}
