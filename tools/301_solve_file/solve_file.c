#include "../tool.h"

#define SOL_BUFFER_LEN   1000
#define MAX_SCR          10000
#define MAX_SCR_LEN      250

char *solver;
int64_t size = 0, N = 0;
char *buf;
char scrambles[MAX_SCR][MAX_SCR_LEN];

void run(void) {
	int64_t i, nsols;
	long long stats[NISSY_SIZE_SOLVE_STATS];
	char sol[SOL_BUFFER_LEN], cube[22];

	printf("Solved the following scrambles:\n\n");
	for (i = 0; i < N; i++) {
		printf("%" PRId64 ". %s\n", i+1, scrambles[i]);
		printf("Solving scramble %s\n", scrambles[i]);
		if (nissy_applymoves(NISSY_SOLVED_CUBE, scrambles[i], cube)
		     == -1) {
			printf("Invalid scramble\n");
			continue;
		}
		nsols = nissy_solve(cube, solver, NISSY_NISSFLAG_NORMAL,
		    0, 20, 1, -1, 0, size, buf, SOL_BUFFER_LEN, sol, stats);
		if (nsols == 0)
			printf("No solution found\n");
		else
			printf("Solutions:\n%s\n", sol);
	}
}

int main(int argc, char **argv) {
	char filename[255], *scrfilename;
	FILE *scrfile;

	if (argc < 3) {
		printf("Error: not enough arguments. "
		    "A solver and a scramble file must be given.\n");
		return 1;
	}

	solver = argv[1];
	scrfilename = argv[2];

	srand(time(NULL));
	nissy_setlogger(log_stderr);

	sprintf(filename, "tables/%s", solver);
	if (getdata(solver, &buf, filename) != 0)
		return 1;

	size = nissy_datasize(solver);

	if ((scrfile = fopen(scrfilename, "r")) == NULL) {
		printf("Error: could not read given file '%s'.\n",
		    scrfilename);
		return 1;
	}

	printf("Reading scrambles from file '%s'.\n", scrfilename);
	for (N = 0; fgets(scrambles[N], MAX_SCR_LEN, scrfile) != NULL; N++) ;
	fclose(scrfile);

	timerun(run);

	free(buf);
	return 0;
}
