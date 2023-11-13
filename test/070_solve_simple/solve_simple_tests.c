#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CUBE_AVX2
#include <immintrin.h>
#endif

#include "../../cube.h"

#define S 10000

int main() {
	char cubestr[S], solverstr[S], optionsstr[S], nisstypestr[S];
	char minmovesstr[S], maxmovesstr[S], maxsolsstr[S], optimalstr[S];
	char solutionsstr[S];
	cube_t cube;
	int64_t maxsols;
	int8_t minmoves, maxmoves, optimal;

	fgets(cubestr, S, stdin);
	fgets(solverstr, S, stdin);
	fgets(optionsstr, S, stdin);
	fgets(nisstypestr, S, stdin);
	fgets(minmovesstr, S, stdin);
	fgets(maxmovesstr, S, stdin);
	fgets(maxsolsstr, S, stdin);
	fgets(optimalstr, S, stdin);

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

	return 0;
}
