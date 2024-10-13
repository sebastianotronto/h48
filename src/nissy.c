#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "nissy.h"
#include "utils/utils.h"
#include "arch/arch.h"
#include "core/core.h"
#include "solvers/solvers.h"

int parse_h48_solver(const char *, uint8_t [static 1], uint8_t [static 1]);
STATIC long long write_result(cube_t, char [static NISSY_SIZE_B32]);
STATIC bool distribution_equal(const uint64_t [static INFO_DISTRIBUTION_LEN],
    const uint64_t [static INFO_DISTRIBUTION_LEN], uint8_t);
STATIC bool checkdata(const char *, const tableinfo_t *);

#define GETCUBE_OPTIONS(S, F) { .option = S, .fix = F }
struct {
	char *option;
	void (*fix)(long long *, long long *, long long *, long long *);
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
checkdata(const char *buf, const tableinfo_t *info)
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

STATIC long long
write_result(cube_t cube, char result[static NISSY_SIZE_B32])
{
	writecube("B32", cube, NISSY_SIZE_B32, result);

	if (!issolvable(cube)) {
		LOG("Warning: resulting cube is not solvable\n");
		return NISSY_WARNING_UNSOLVABLE;
	}

	return NISSY_OK;
}

long long
nissy_compose(
	const char cube[static NISSY_SIZE_B32],
	const char permutation[static NISSY_SIZE_B32],
	char result[static NISSY_SIZE_B32]
)
{
	cube_t c, p, res;
	long long err;

	c = readcube("B32", cube);

	if (!isconsistent(c)) {
		LOG("Error in nissy_compose: given cube is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_compose_error;
	}

	p = readcube("B32", permutation);

	if (!isconsistent(p)) {
		LOG("Error in nissy_compose: given permutation is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_compose_error;
	}

	res = compose(c, p);

	if (!isconsistent(res)) {
		LOG("Unknown error: resulting cube is invalid\n");
		err = NISSY_ERROR_UNKNOWN;
		goto nissy_compose_error;
	}

	return write_result(res, result);

nissy_compose_error:
	writecube("B32", ZERO_CUBE, NISSY_SIZE_B32, result);
	return err;
}

long long
nissy_inverse(
	const char cube[static NISSY_SIZE_B32],
	char result[static NISSY_SIZE_B32]
)
{
	cube_t c, res;
	long long err;

	c = readcube("B32", cube);

	if (iserror(c)) {
		LOG("Error in nissy_inverse: given cube is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_inverse_error;
	}

	res = inverse(c);

	if (!isconsistent(res)) {
		LOG("Unknown error: inverted cube is invalid\n");
		err = NISSY_ERROR_UNKNOWN;
		goto nissy_inverse_error;
	}

	return write_result(res, result);

nissy_inverse_error:
	writecube("B32", ZERO_CUBE, NISSY_SIZE_B32, result);
	return err;
}

long long
nissy_applymoves(
	const char cube[static NISSY_SIZE_B32],
	const char *moves,
	char result[static NISSY_SIZE_B32]
)
{
	cube_t c, res;
	long long err;

	if (moves == NULL) {
		LOG("Error: 'moves' argument is NULL\n");
		err = NISSY_ERROR_NULL_POINTER;
		goto nissy_applymoves_error;
	}

	c = readcube("B32", cube);

	if (!isconsistent(c)) {
		LOG("Error in nissy_applymoves: given cube is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_applymoves_error;
	}

	res = applymoves(c, moves);

	if (!isconsistent(res)) {
		/* Assume we got a reasonable error message from applymoves */
		err = NISSY_ERROR_INVALID_MOVES;
		goto nissy_applymoves_error;
	}

	return write_result(res, result);

nissy_applymoves_error:
	writecube("B32", ZERO_CUBE, NISSY_SIZE_B32, result);
	return err;
}

long long
nissy_applytrans(
	const char cube[static NISSY_SIZE_B32],
	const char transformation[static NISSY_SIZE_TRANSFORMATION],
	char result[static NISSY_SIZE_B32]
)
{
	cube_t c, res;
	long long err;

	c = readcube("B32", cube);

	if (!isconsistent(c)) {
		LOG("Error in nissy_applytrans: given cube is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_applytrans_error;
	}

	res = applytrans(c, transformation);

	if (!isconsistent(res)) {
		/* Assume we got a reasonable error message from applytrans */
		err = NISSY_ERROR_INVALID_TRANS;
		goto nissy_applytrans_error;
	}

	return write_result(res, result);

nissy_applytrans_error:
	writecube("B32", ZERO_CUBE, NISSY_SIZE_B32, result);
	return err;
}

long long
nissy_convert(
	const char *format_in,
	const char *format_out,
	const char *cube_string,
        unsigned result_size,
	char result[result_size]
)
{
	cube_t c;
	long long err;

	if (format_in == NULL) {
		LOG("Error: 'format_in' argument is NULL\n");
		err = NISSY_ERROR_NULL_POINTER;
		goto nissy_convert_error;
	}

	if (format_out == NULL) {
		LOG("Error: 'format_out' argument is NULL\n");
		err = NISSY_ERROR_NULL_POINTER;
		goto nissy_convert_error;
	}

	if (cube_string == NULL) {
		LOG("Error: 'cube_string' argument is NULL\n");
		err = NISSY_ERROR_NULL_POINTER;
		goto nissy_convert_error;
	}

	c = readcube(format_in, cube_string);

	if (!isconsistent(c)) {
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_convert_error;
	}

	return writecube(format_out, c, result_size, result);

nissy_convert_error:
	result[0] = '\0';
	return err;
}

long long
nissy_getcube(
	long long ep,
	long long eo,
	long long cp,
	long long co,
	const char *options,
	char result[static NISSY_SIZE_B32]
)
{
	int i;
	cube_t c;

	if (options == NULL) {
		LOG("Error: 'options' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	for (i = 0; getcube_options[i].option != NULL; i++)
		if (!strcmp(options, getcube_options[i].option))
			getcube_options[i].fix(&ep, &eo, &cp, &co);

	c = getcube(ep, eo, cp, co);

	if (!isconsistent(c)) {
		LOG("Error: could not get cube with ep=%" PRId64 ", eo=%"
		    PRId64 ", cp=%" PRId64 ", co=%" PRId64 ".\n",
		    ep, eo, cp, co);
		return NISSY_ERROR_OPTIONS;
	}

	return write_result(c, result);
}

long long
nissy_datasize(
	const char *solver
)
{
	if (solver == NULL) {
		LOG("Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	/* gendata() handles a NULL *data as a "dryrun" request */
	return nissy_gendata(solver, 0, NULL);
}

long long
nissy_datainfo(
        uint64_t data_size,
	const char data[data_size],
	void (*write)(const char *, ...)
)
{
	uint8_t i;
	tableinfo_t info;
	long long ret;

	ret = readtableinfo(data_size, data, &info);
	if (ret != 0)
		return ret;

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
		return NISSY_ERROR_DATA;
	}

	if (info.next != 0)
		return nissy_datainfo(
		    data_size - info.next, (char *)data + info.next, write);

	write("\n---------\n");

	return NISSY_OK;
}

long long
nissy_gendata(
	const char *solver,
	unsigned long long data_size,
	char data[data_size]
)
{
	int p;
	gendata_h48_arg_t arg;

	if (solver == NULL) {
		LOG("Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	arg.buf_size = data_size;
	arg.buf = data;
	if (!strncmp(solver, "h48", 3)) {
		p = parse_h48_solver(solver, &arg.h, &arg.k);
		arg.maxdepth = 20;
		if (p != 0)
			return NISSY_ERROR_UNKNOWN;
		return gendata_h48(&arg);
	} else {
		LOG("gendata: unknown solver %s\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
}

long long
nissy_checkdata(
	unsigned long long data_size,
	const char data[data_size]
)
{
	char *buf;
	tableinfo_t info;

	for (buf = (char *)data;
	     readtableinfo(data_size, buf, &info);
	     buf += info.next, data_size -= info.next)
	{
		if (!checkdata(buf, &info)) {
			LOG("Error: data for %s is inconsistent with info!\n",
			    info.solver);
			return NISSY_ERROR_DATA;
		}
		if (info.next == 0)
			break;
	}

	return NISSY_OK;
}

long long
nissy_solve(
	const char cube[static NISSY_SIZE_B32],
	const char *solver, 
	unsigned nissflag,
	unsigned minmoves,
	unsigned maxmoves,
	unsigned maxsols,
	int optimal,
	unsigned long long data_size,
	const char data[data_size],
	unsigned sols_size,
	char sols[sols_size]
)
{
	cube_t c;
	int p;
	uint8_t h, k;

	if (solver == NULL) {
		LOG("Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	c = readcube_B32(cube);

	if (!isconsistent(c)) {
		LOG("solve: cube is invalid\n");
		return NISSY_ERROR_INVALID_CUBE;
	}

	if (!issolvable(c)) {
		LOG("solve: cube is not solvable\n");
		return NISSY_ERROR_UNSOLVABLE_CUBE;
	}

	if (maxsols == 0) {
		LOG("solve: 'maxsols' is 0, returning no solution\n");
		return 0;
	}

	if (!strncmp(solver, "h48", 3)) {
		if (!strcmp(solver, "h48stats"))
			return solve_h48stats(c, maxmoves, data, sols);

		p = parse_h48_solver(solver, &h, &k);
		if (p != 0) {
			LOG("solve: unknown solver %s\n", solver);
			return NISSY_ERROR_INVALID_SOLVER;
		} else {
			return THREADS > 1 ?
			    solve_h48_multithread(c, minmoves, maxmoves,
			        maxsols, data_size, data, sols_size, sols) :
			    solve_h48(c, minmoves, maxmoves, maxsols,
			        data_size, data, sols_size, sols);
		}
	} else {
		LOG("solve: unknown solver '%s'\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
}

long long
nissy_setlogger(
	void (*log)(const char *, ...)
)
{
	nissy_log = log;
	return NISSY_OK;
}
