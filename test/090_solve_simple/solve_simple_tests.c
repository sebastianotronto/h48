#include "../test.h"

int64_t solve(cube_t, char *, char *, char *, int8_t, int8_t, int64_t, int8_t,
    void *, char *);

void run(void) {
	char cubestr[STRLENMAX], solverstr[STRLENMAX], optionsstr[STRLENMAX];
	char nisstypestr[STRLENMAX], minmovesstr[STRLENMAX];
	char maxmovesstr[STRLENMAX], maxsolsstr[STRLENMAX];
	char optimalstr[STRLENMAX], solutionsstr[STRLENMAX];
	cube_t cube;
	int64_t maxsols;
	int8_t minmoves, maxmoves, optimal;

	fgets(cubestr, STRLENMAX, stdin);
	fgets(solverstr, STRLENMAX, stdin);
	fgets(optionsstr, STRLENMAX, stdin);
	fgets(nisstypestr, STRLENMAX, stdin);
	fgets(minmovesstr, STRLENMAX, stdin);
	fgets(maxmovesstr, STRLENMAX, stdin);
	fgets(maxsolsstr, STRLENMAX, stdin);
	fgets(optimalstr, STRLENMAX, stdin);

	solverstr[strcspn(solverstr, "\n")] = 0;
	optionsstr[strcspn(optionsstr, "\n")] = 0;
	nisstypestr[strcspn(nisstypestr, "\n")] = 0;

	cube = readcube("H48", cubestr);
	minmoves = atoi(minmovesstr);
	maxmoves = atoi(maxmovesstr);
	maxsols = atoi(maxsolsstr);
	optimal = atoi(optimalstr);

	solve(
		cube,
		solverstr,
		optionsstr,
		nisstypestr,
		minmoves,
		maxmoves,
		maxsols,
		optimal,
		NULL,
		solutionsstr
	);

	printf("%s", solutionsstr);
}
