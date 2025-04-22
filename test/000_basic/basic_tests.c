#include "../test.h"

bool issolved(oriented_cube_t);
bool equal(cube_t, cube_t);

void
check(oriented_cube_t cube, char *name)
{
	printf("%s is%s solvable\n", name, issolvable(cube) ? "" : " NOT");
	printf("%s is%s solved\n", name, issolved(cube) ? "" : " NOT");
}

void
check2(oriented_cube_t cube1, char *name1, oriented_cube_t cube2, char *name2)
{
	bool eq = equal(cube1.cube, cube2.cube) &&
	    cube1.orientation == cube2.orientation;
	printf("%s and %s are%s equal\n", name1, name2, eq ? "" : " NOT");
}

void run(void) {
	oriented_cube_t zero, solved;

	memset(&zero, 0, sizeof(oriented_cube_t));
	solved = solvedcube();

	check(solved, "Solved");
	check(zero, "Zero");

	check2(solved, "Solved", solved, "Solved");
	check2(solved, "Solved", zero, "Zero");
	check2(zero, "Zero", solved, "Solved");
}
