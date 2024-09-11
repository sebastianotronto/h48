#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "utils/utils.h"
#include "arch/arch.h"
#include "core/core.h"
#include "solvers/solvers.h"

#include "nissy.h"

STATIC int parse_h48_options(const char *, uint8_t *, uint8_t *, uint8_t *);
STATIC int64_t write_result(cube_t, char [static 22]);

/* TODO: add option to get DR, maybe C-only, E-only, eo... */
#define GETCUBE_OPTIONS(S, F) { .option = S, .fix = F }
struct {
	char *option;
	void (*fix)(int64_t *, int64_t *, int64_t *, int64_t *);
} getcube_options[] = {
	GETCUBE_OPTIONS("fix", getcube_fix),
	GETCUBE_OPTIONS(NULL, NULL)
};

STATIC int
parse_h48_options(const char *buf, uint8_t *h, uint8_t *k, uint8_t *maxdepth)
{
	bool h_valid, k_valid, maxdepth_valid;
	int i;

	/* TODO temporarily, options are in the form "h;k;maxdepth" */
	if (h != NULL)
		*h = atoi(buf);
	h_valid = h == NULL || *h <= 11;

	for (i = 0; buf[i] != ';'; i++)
		if (buf[i] == 0)
			goto parse_h48_options_error;

	if (k != NULL)
		*k = atoi(&buf[i+1]);
	k_valid = k == NULL || (*k == 2 || *k == 4);

	for (i = i+1; buf[i] != ';'; i++)
		if (buf[i] == 0)
			goto parse_h48_options_error;

	if (maxdepth != NULL)
		*maxdepth = atoi(&buf[i+1]);
	maxdepth_valid = maxdepth == NULL || *maxdepth <= 20;

	return h_valid && k_valid && maxdepth_valid ? 0 : 1;

parse_h48_options_error:
	*h = 0;
	*k = 0;
	*maxdepth = 0;
	LOG("Error parsing options: must be in \"h;k;maxdepth\" format "
	    " (instead it was \"%s\")\n", buf);
	return -1;
}

STATIC int64_t
write_result(cube_t cube, char result[static 22])
{
	if (!isconsistent(cube)) {
		writecube("B32", ZERO_CUBE, result);
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

	res = applymoves(SOLVED_CUBE, moves);

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
nissy_datainfo(
	const void *table,
	void (*write)(const char *, ...)
)
{
	uint8_t i;
	tableinfo_t info;

	readtableinfo(table, &info);

	write("\n---------\n\n");
	write("Table information for '%s'\n", info.solver);
	write("\n");
	write("Size:      %" PRIu64 " bytes\n", info.fullsize);
	write("Entries:   %" PRIu64 " (%" PRIu8 " bits per entry)",
	    info.entries, info.bits);
	write("\n");

	switch (info.type) {
	case TABLETYPE_PRUNING:
		write("\n");
		if (info.base != 0)
			write(" (base value = %" PRIu8 ")", info.base);
		write(":\nValue\tPositions\n");
		for (i = 0; i <= info.maxvalue; i++) {
			write("%" PRIu8 "\t%" PRIu64 "\n",
			    i, info.distribution[i]);
		}
		break;
	case TABLETYPE_SPECIAL:
		write("This is an ad-hoc table\n");
		break;
	default:
		LOG("datainfo: unknown table type\n");
		return 1;
	}

	if (info.next != 0) {
		return nissy_datainfo((char *)table + info.next, write);
	}

	write("\n---------\n");

	return 0;
}

int64_t
nissy_gendata(
	const char *solver,
	const char *options,
	void *data
)
{
	int p;
	int64_t ret;
	gendata_h48_arg_t arg;

	arg.buf = data;
	if (!strcmp(solver, "h48")) {
		p = parse_h48_options(options, &arg.h, &arg.k, &arg.maxdepth);
		if (p != 0) {
			LOG("gendata: could not parse options\n");
			ret = -1;
		} else {
			ret = gendata_h48(&arg);
		}
	} else if (!strcmp(solver, "h48stats")) {
		arg.h = 0;
		arg.k = 4;
		arg.maxdepth = 20;
		ret = gendata_h48(&arg);
	} else {
		LOG("gendata: implemented only for h48 solver\n");
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
	int p;
	int64_t ret;
	uint8_t h, k;

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
	if (!strcmp(solver, "h48")) {
		p = parse_h48_options(options, &h, &k, NULL);
		if (p != 0) {
			LOG("gendata: could not parse options\n");
			ret = -1;
		} else {
			ret = solve_h48(c, minmoves, maxmoves, maxsolutions,
			    h, k, data, solutions);
		}
	} else if (!strcmp(solver, "h48stats")) {
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
