#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "utils/utils.h"
#include "arch/arch.h"
#include "core/core.h"
#include "solvers/solvers.h"

#include "nissy.h"

int parse_h48_solver(const char *, uint8_t [static 1], uint8_t [static 1]);
STATIC int64_t write_result(cube_t, char [static 22]);
STATIC bool distribution_equal(const uint64_t [static INFO_DISTRIBUTION_LEN],
    const uint64_t [static INFO_DISTRIBUTION_LEN], uint8_t);
STATIC bool checkdata(const void *, const tableinfo_t *);

#define GETCUBE_OPTIONS(S, F) { .option = S, .fix = F }
struct {
	char *option;
	void (*fix)(int64_t *, int64_t *, int64_t *, int64_t *);
} getcube_options[] = {
	GETCUBE_OPTIONS("fix", getcube_fix),
	GETCUBE_OPTIONS(NULL, NULL)
};

int
parse_h48_solver(const char *buf, uint8_t h[static 1], uint8_t k[static 1])
{
	const char *fullbuf = buf;

	buf += 3;

	if (!strcmp(buf, "stats")) {
		*h = 0;
		*k = 4;
		return 0;
	}

	if (*buf != 'h')
		goto parse_h48_solver_error;
	buf++;

	*h = atoi(buf);

	for ( ; *buf >= 0 + '0' && *buf <= 9 + '0'; buf++)
		if (*buf == 0)
			goto parse_h48_solver_error;

	if (*buf != 'k')
		goto parse_h48_solver_error;
	buf++;

	*k = atoi(buf);

	return *h < 12 && (*k == 2 || (*k == 4 && *h == 0)) ? 0 : 1;

parse_h48_solver_error:
	*h = 0;
	*k = 0;
	LOG("Error parsing solver: must be in \"h48h*k*\" format"
	    " or \"h48stats\", but got %s\n", fullbuf);
	return -1;
}

STATIC bool
checkdata(const void *buf, const tableinfo_t *info)
{
	uint64_t distr[INFO_DISTRIBUTION_LEN];

	if (!strncmp(info->solver, "cocsep", 6)) {
		getdistribution_cocsep(
		    (uint32_t *)((char *)buf + INFOSIZE), distr);
	} else if (!strncmp(info->solver, "h48", 3)) {
		getdistribution_h48((uint8_t *)buf + INFOSIZE, distr,
		    info->h48h, info->bits);
	} else {
		LOG("checkdata: unknown solver %s\n", info->solver);
		return false;
	}

	return distribution_equal(info->distribution, distr, info->maxvalue);
}

STATIC bool
distribution_equal(
	const uint64_t expected[static INFO_DISTRIBUTION_LEN],
	const uint64_t actual[static INFO_DISTRIBUTION_LEN],
	uint8_t maxvalue
)
{
	int wrong;
	uint8_t i;

	for (i = 0, wrong = 0; i <= MAX(maxvalue, 20); i++) {
		if (expected[i] != actual[i]) {
			wrong++;
			LOG("Value %" PRIu8 ": expected %" PRIu64 ", found %"
			    PRIu64 "\n", i, expected[i], actual[i]);
		}
	}

	return wrong == 0;
}

STATIC int64_t
write_result(cube_t cube, char result[static 22])
{
	if (!isconsistent(cube)) {
		LOG("Error: resulting cube is invalid\n");
		writecube("B32", ZERO_CUBE, result);
		return 8;
	}

	writecube("B32", cube, result);

	if (!issolvable(cube)) {
		LOG("Warning: resulting cube is not solvable\n");
		return 9;
	}

	return 0;
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

	if (!isconsistent(c)) {
		LOG("Error in nissy_compose: given cube is invalid\n");
		return 1;
	}

	p = readcube("B32", permutation);

	if (!isconsistent(p)) {
		LOG("Error in nissy_compose: given permutation is invalid\n");
		return 2;
	}

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

	if (iserror(c)) {
		LOG("Error in nissy_inverse: given cube is invalid\n");
		return 1;
	}

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

	if (!isconsistent(c)) {
		LOG("Error in nissy_applymoves: given cube is invalid\n");
		return 1;
	}

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

	if (!isconsistent(c)) {
		LOG("Error in nissy_applytrans: given cube is invalid\n");
		return 1;
	}

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

	if (!isconsistent(res)) {
		/* Moves must be invalid */
		return 1;
	}

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
	int ret;
	cube_t c;

	c = readcube(format_in, cube_string);

	if (iserror(c))
		return 1;

	ret = writecube(format_out, c, result);

	if (ret != 0)
		return 2;

	return isconsistent(c) ? 0 : 3;
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
	const char *solver
)
{
	/* gendata() handles a NULL *data as a "dryrun" request */
	return nissy_gendata(solver, NULL);
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
		write("Table distribution:\nValue\tPositions\n");
		for (i = 0; i <= info.maxvalue; i++) {
			write("%" PRIu8 "\t%" PRIu64 "\n",
			    i + info.base, info.distribution[i]);
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
	void *data
)
{
	int p;
	int64_t ret;
	gendata_h48_arg_t arg;

	arg.buf = data;
	if (!strncmp(solver, "h48", 3)) {
		p = parse_h48_solver(solver, &arg.h, &arg.k);
		arg.maxdepth = 20;
		ret = p == 0 ? (int64_t)gendata_h48(&arg) : -1;
	} else {
		LOG("gendata: unknown solver %s\n", solver);
		ret = -1;
	}

	return ret;
}

int64_t
nissy_checkdata(
	const char *solver,
	const void *data
)
{
	char *buf;
	tableinfo_t info;

	for (buf = (char *)data; readtableinfo(buf, &info); buf += info.next) {
		if (!checkdata(buf, &info)) {
			LOG("Error: data for %s is inconsistent with info!\n",
			    info.solver);
			return 1;
		}
		if (info.next == 0)
			break;
	}

	return 0;
}

int64_t
nissy_solve(
	const char cube[static 22],
	const char *solver, 
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

	if (!strncmp(solver, "h48", 3)) {
		if (!strcmp(solver, "h48stats"))
			return solve_h48stats(c, maxmoves, data, solutions);

		p = parse_h48_solver(solver, &h, &k);
		if (p != 0) {
			return -1;
		} else {
			return THREADS > 1 ?
				solve_h48_multithread(c, minmoves,
				    maxmoves, maxsolutions, data, solutions) :
				solve_h48(c, minmoves,
				    maxmoves, maxsolutions, data, solutions);
		}
	} else if (!strcmp(solver, "simple")) {
		return solve_simple(
		    c, minmoves, maxmoves, maxsolutions, optimal, solutions);
	} else {
		LOG("solve: unknown solver '%s'\n", solver);
		return -1;
	}
}

void
nissy_setlogger(void (*log)(const char *, ...))
{
	nissy_log = log;
}
