#include "../test.h"

bool issolved(cube_t);
bool equal(cube_t, cube_t);

void
check(cube_t cube, char *name)
{
	printf("%s is%s solvable\n", name, issolvable(cube) ? "" : " NOT");
	printf("%s is%s solved\n", name, issolved(cube) ? "" : " NOT");
}

void
check2(cube_t cube1, char *name1, cube_t cube2, char *name2)
{
	printf("%s and %s are%s equal\n", name1, name2,
	    equal(cube1, cube2) ? "" : " NOT");
}

int main() {
	cube_t zero, solved;

	memset(&zero, 0, sizeof(cube_t));
	solved = solvedcube();
	check(solved, "Solved");
	check(zero, "Zero");

	check2(solved, "Solved", solved, "Solved");
	check2(solved, "Solved", zero, "Zero");
	check2(zero, "Zero", solved, "Solved");

	return 0;
}
