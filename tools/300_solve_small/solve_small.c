#include "../tool.h"

#define SOL_BUFFER_LEN 1000

char *solver;
int64_t size = 0;
char *buf;

char *scrambles[] = {
	//"R D' R2 D R U2 R' D' R U2 R D R'", /* 12 optimal */
	//"RLUD RLUD RLUD", /* 12 optimal */
	"R' U' F D2 L2 F R2 U2 R2 B D2 L B2 D' B2 L' R' B D2 B U2 L U2 R' U' F", /* FMC2019 A1 - 16 optimal */
	// "R' U' F D R F2 D L F D2 F2 L' U R' L2 D' R2 F2 R2 D L2 U2 R' U' F", /* FMC2024 A1 - 19 optimal */
	NULL
};

void run(void) {
	int i;
	int64_t n;
	char sol[SOL_BUFFER_LEN], cube[22];

	printf("Solved the following scrambles:\n\n");
	for (i = 0; scrambles[i] != NULL; i++) {
		printf("%d. %s\n", i+1, scrambles[i]);
		printf("Solving scramble %s\n", scrambles[i]);
		if (nissy_applymoves(NISSY_SOLVED_CUBE, scrambles[i], cube)
		     == -1) {
			printf("Invalid scramble\n");
			continue;
		}
		n = nissy_solve(cube, solver, NISSY_NISSFLAG_NORMAL,
		    0, 20, 1, -1, size, buf, SOL_BUFFER_LEN, sol);
		if (n == 0)
			printf("No solution found\n");
		else
			printf("Solutions:\n%s\n", sol);
	}
}

int main(int argc, char **argv) {
	char filename[255];

	if (argc < 2) {
		printf("Error: not enough arguments. "
		    "A solver must be given.\n");
		return 1;
	}

	solver = argv[1];
	srand(time(NULL));
	nissy_setlogger(log_stderr);


	sprintf(filename, "tables/%s", solver);
	if (getdata(solver, &buf, filename) != 0)
		return 1;

	size = nissy_datasize(solver);

	timerun(run);

	free(buf);
	return 0;
}
