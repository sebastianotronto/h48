#include "../tool.h"

#define SOL_BUFFER_LEN 10000

typedef struct {
	char *scramble;
	char *solutions;
} scramblesol_t;

char *solver;
int64_t size = 0;
char *buf;

scramblesol_t s[] = {
[0] = {
	.scramble = "R' D R U R' D' R U'",
	.solutions = "U R' D R U' R' D' R\n"
	     "B' D2 B U' B' D2 B U\n",
},
{
	.scramble = "", /* End-of-list signal */
}
};

bool check_one(char *actual, char *expected) {
	unsigned i;
	size_t l_actual, l_expected;

	l_actual = strlen(actual);
	l_expected = strlen(expected);
	if (l_actual > l_expected)
		return false;
	for (i = 0; i < l_expected; i++) {
		if (!strncmp(actual, &expected[i], l_actual))
			return true;
		while(expected[i] != '\n') i++;
	}
	return false;
}

bool check_all(char *actual, char *expected) {
	unsigned i, found, n_expected;
	size_t l_actual;

	if (strlen(actual) != strlen(expected))
		return false;

	for (i = 0, n_expected = 0; expected[i]; i++)
		n_expected += expected[i] == '\n';

	l_actual = strlen(actual);
	for (i = 0, found = 0; i < l_actual; i++)
		found += check_one(&actual[i], expected);

	return found == n_expected;
}

void run(void) {
	int i;
	int64_t n;
	long long stats[NISSY_SIZE_SOLVE_STATS];
	char sol[SOL_BUFFER_LEN], cube[22];

	for (i = 0; s[i].scramble[0]; i++) {
		printf("\n%d. %s\n", i, s[i].scramble);

		/* Single solution */
		if (nissy_applymoves(NISSY_SOLVED_CUBE, s[i].scramble, cube)
		     == -1) {
			printf("Invalid scramble\n");
			continue;
		}
		n = nissy_solve(cube, solver, NISSY_NISSFLAG_NORMAL,
		    0, 20, 1, -1, 0, size, buf, SOL_BUFFER_LEN, sol, stats);
		if (n == 0) {
			printf("Error! No solution found for scramble %s\n",
			    s[i].scramble);
			return;
		}
		if (check_one(sol, s[i].solutions)) {
			printf("Single solution for scramble %d is correct\n", i);
		} else {
			printf("Error! Scramble %d: %s\n", i, s[i].scramble);
			printf("Found solution(s):\n%s", sol);
			printf("Valid solution(s):\n%s", s[i].solutions);
			return;
		}

		/* Multiple solutions */
		if (nissy_applymoves(NISSY_SOLVED_CUBE, s[i].scramble, cube)
		     == -1) {
			printf("Invalid scramble\n");
			continue;
		}
		n = nissy_solve(cube, solver, NISSY_NISSFLAG_NORMAL,
		    0, 20, 100, 0, 0, size, buf, SOL_BUFFER_LEN, sol, stats);
		if (check_all(sol, s[i].solutions)) {
			printf("All solutions for scramble %d are correct\n", i);
		} else {
			printf("Error! Scramble %s\n", s[i].scramble);
			printf("Found solution(s):\n%s", sol);
			printf("Valid solution(s):\n%s", s[i].solutions);
			return;
		}
	}

	printf("\nAll scrambles solved correctly\n");
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
