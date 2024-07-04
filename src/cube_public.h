#include "cube.h"

_static int64_t write_result(cube_t, char [static 22]);

/* TODO: add option to get DR, maybe C-only, E-only, eo... */
#define GETCUBE_OPTIONS(S, F) { .option = S, .fix = F }
struct {
	char *option;
	void (*fix)(int64_t *, int64_t *, int64_t *, int64_t *);
} getcube_options[] = {
	GETCUBE_OPTIONS("fix", getcube_fix),
	GETCUBE_OPTIONS(NULL, NULL)
};

_static int64_t
write_result(cube_t cube, char result[static 22])
{
	if (!isconsistent(cube)) {
		writecube("B32", zero, result);
		return 2;
	}

	writecube("B32", cube, result);

	return issolvable(cube) ? 0 : 1;
}

int64_t
nissy_compose(
	const char cube[static 22],
	const char permutation[static 22],
	char result[static 22]
)
{
	cube_t c, p, res;

	c = readcube("B32", cube);
	p = readcube("B32", permutation);
	res = compose(c, p);

	return write_result(res, result);
}

int64_t
nissy_inverse(
	const char cube[static 22],
	char result[static 22]
)
{
	cube_t c, res;

	c = readcube("B32", cube);
	res = inverse(c);

	return write_result(res, result);
}

int64_t
nissy_applymoves(
	const char cube[static 22],
	const char *moves,
	char result[static 22]
)
{
	cube_t c, res;

	c = readcube("B32", cube);
	res = applymoves(c, moves);

	return write_result(res, result);
}

int64_t
nissy_applytrans(
	const char cube[static 22],
	const char *transformation,
	char result[static 22]
)
{
	cube_t c, res;

	c = readcube("B32", cube);
	res = applytrans(c, transformation);

	return write_result(res, result);
}

int64_t
nissy_frommoves(
	const char *moves,
	char result[static 22]
)
{
	cube_t res;

	res = applymoves(solved, moves);

	return write_result(res, result);
}

int64_t
nissy_convert(
	const char *format_in,
	const char *format_out,
	const char *cube_string,
	char *result
)
{
	cube_t c;

	c = readcube(format_in, cube_string);
	writecube(format_out, c, result);

	return isconsistent(c) ? 0 : 2;
}

int64_t
nissy_getcube(
	int64_t ep,
	int64_t eo,
	int64_t cp,
	int64_t co,
	const char *options,
	char result[static 22]
)
{
	int i;
	cube_t c;

	for (i = 0; getcube_options[i].option != NULL; i++)
		if (!strcmp(options, getcube_options[i].option))
			getcube_options[i].fix(&ep, &eo, &cp, &co);

	c = getcube(ep, eo, cp, co);

	return write_result(c, result);
}

int64_t
nissy_datasize(
	const char *solver,
	const char *options
)
{
	/* gendata() handles a NULL *data as a "dryrun" request */
	return nissy_gendata(solver, options, NULL);
}

int64_t
nissy_gendata(
	const char *solver,
	const char *options,
	void *data
)
{
	int64_t ret;
	uint8_t maxdepth, h, i, j;

	if (!strcmp(solver, "H48")) {
		/*  options are in the form "h;maxdepth" */
		for (i = 0; options[i] != ';'; i++) ;
		for (j = i; options[j]; j++) ;
		h = atoi(options);
		maxdepth = atoi(&options[i+1]);
		ret = gendata_h48(data, h, maxdepth);
	} else if (!strcmp(solver, "H48stats")) {
		ret = gendata_cocsep(data, NULL, NULL);
	} else {
		LOG("gendata: implemented only for H48 solver\n");
		ret = -1;
	}

	return ret;
}

int64_t
nissy_solve(
	const char cube[static 22],
	const char *solver, 
	const char *options,
	const char *nisstype,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsolutions,
	int8_t optimal,
	const void *data,
	char *solutions
)
{
	cube_t c;
	int64_t ret;
	int h;

	c = readcube_B32(cube);

	if (!issolvable(c)) {
		LOG("solve: cube is not solvable\n");
		return -1;
	}

	if (minmoves < 0) {
		LOG("solve: 'minmoves' is negative, setting it to 0\n");
		minmoves = 0;
	}

	if (maxmoves < 0) {
		LOG("solve: 'maxmoves' is negative, setting it to 20\n");
		maxmoves = 20;
	}

	if (maxsolutions < 0) {
		LOG("solve: 'maxsols' is negative, stopping\n");
		return -1;
	}

	if (maxsolutions == 0) {
		LOG("solve: 'maxsols' is 0, returning no solution\n");
		return 0;
	}

	if (solutions == NULL) {
		LOG("solve: return parameter 'solutions' is NULL, stopping\n");
		return -1;
	}

	/* TODO define and use solve_options_t */
	if (!strcmp(solver, "H48")) {
		h = atoi(options); /* TODO: better parsing */
		ret = solve_h48(
		    c, minmoves, maxmoves, maxsolutions,
		    (uint8_t)h, data, solutions);
		ret = -1;
	} else if (!strcmp(solver, "H48stats")) {
		ret = solve_h48stats(c, maxmoves, data, solutions);
	} else if (!strcmp(solver, "simple")) {
		ret = solve_simple(
		    c, minmoves, maxmoves, maxsolutions, optimal, solutions);
	} else {
		LOG("solve: unknown solver '%s'\n", solver);
		ret = -1;
	}

	return ret;
}

void
nissy_setlogger(void (*log)(const char *, ...))
{
	nissy_log = log;
}
