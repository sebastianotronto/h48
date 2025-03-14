#include <inttypes.h>
#include <limits.h>
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

long long parse_h48_solver(
    const char *, uint8_t [static 1], uint8_t [static 1]);
STATIC bool checkdata(const char *, const tableinfo_t *);
STATIC bool distribution_equal(const uint64_t [static INFO_DISTRIBUTION_LEN],
    const uint64_t [static INFO_DISTRIBUTION_LEN], uint8_t);
STATIC long long write_result(cube_t, char [static NISSY_SIZE_B32]);
STATIC size_t my_strnlen(const char *, size_t); 
STATIC long long nissy_dataid(const char *, char [static NISSY_DATAID_SIZE]);
STATIC long long nissy_gendata_unsafe(
    const char *, unsigned long long, char *);

#define GETCUBE_OPTIONS(S, F) { .option = S, .fix = F }
struct {
	char *option;
	void (*fix)(long long *, long long *, long long *, long long *);
} getcube_options[] = {
	GETCUBE_OPTIONS("fix", getcube_fix),
	GETCUBE_OPTIONS(NULL, NULL)
};

long long
parse_h48_solver(const char *buf, uint8_t h[static 1], uint8_t k[static 1])
{
	const char *fullbuf = buf;

	buf += 3;

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
	LOG("Error parsing solver: must be in \"h48h*k*\" format,"
	    " but got %s\n", fullbuf);
	return NISSY_ERROR_INVALID_SOLVER;
}

STATIC bool
checkdata(const char *buf, const tableinfo_t *info)
{
	uint64_t distr[INFO_DISTRIBUTION_LEN];

	if (info == NULL || my_strnlen(info->solver, INFO_SOLVER_STRLEN)
	    == INFO_SOLVER_STRLEN) {
		LOG("checkdata: error reading table info\n");
		return false;
	} else if (!strncmp(info->solver, "cocsep", 6)) {
		getdistribution_cocsep(
		    (uint32_t *)((char *)buf + INFOSIZE), distr);
	} else if (!strncmp(info->solver, "h48", 3)) {
		getdistribution_h48((uint8_t *)buf + INFOSIZE, distr,
		    info->h48h, info->bits);
	} else if (!strncmp(info->solver, "coordinate solver for ", 22)) {
		getdistribution_coord((uint8_t *)buf + INFOSIZE,
		    info->solver + 22, distr);
	} else if (!strncmp(info->solver, "eoesep data for h48", 19)) {
		return true;
	} else if (!strncmp(info->solver, "coord helper table for ", 23)) {
		return true;
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

STATIC size_t
my_strnlen(const char *str, size_t maxlen)
{
	size_t i;

	for (i = 0; i < maxlen; i++)
		if (str[i] == '\0')
			return i;

	return maxlen;
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
nissy_datainfo(
        uint64_t data_size,
	const char data[data_size],
	void (*write)(const char *, ...)
)
{
	uint8_t i;
	tableinfo_t info;
	long long ret;

	if ((size_t)data % 8 != 0) {
		LOG("nissy_datainfo: buffere is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

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

STATIC long long
nissy_dataid(const char *solver, char dataid[static NISSY_DATAID_SIZE])
{
	if (!strncmp(solver, "h48", 3)) {
		uint8_t h, k;
		long long err;
		if ((err = parse_h48_solver(solver, &h, &k)) != NISSY_OK)
			return err;
		/* TODO: also check that h and k are admissible */
		else strcpy(dataid, solver);
		return err;
		/* TODO: do this when moved parser */
		/* return dataid_h48(solver, dataid); */
	} else if (!strncmp(solver, "coord_", 6)) {
		return dataid_coord(solver+6, dataid);
	} else {
		LOG("gendata: unknown solver %s\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
}

long long
nissy_solverinfo(
	const char *solver,
	char dataid[static NISSY_DATAID_SIZE]
)
{
	long long err;
	if ((err = nissy_dataid(solver, dataid)) != NISSY_OK)
		return err;

	/* gendata() handles a NULL *data as a "dryrun" request */
	return nissy_gendata_unsafe(solver, 0, NULL);
}

long long
nissy_gendata(
	const char *solver,
	unsigned long long data_size,
	char data[data_size]
)
{
	return nissy_gendata_unsafe(solver, data_size, data);
}

STATIC long long
nissy_gendata_unsafe(
	const char *solver,
	unsigned long long data_size,
	char *data
)
{
	long long parse_ret;
	gendata_h48_arg_t arg;

	if (solver == NULL) {
		LOG("Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	if ((size_t)data % 8 != 0) {
		LOG("nissy_gendata: buffere is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	if (!strncmp(solver, "h48", 3)) {
		arg.buf_size = data_size;
		arg.buf = data;
		parse_ret = parse_h48_solver(solver, &arg.h, &arg.k);
		arg.maxdepth = 20;
		if (parse_ret != NISSY_OK)
			return parse_ret;
		return gendata_h48(&arg);
	} else if (!strncmp(solver, "coord_", 6)) {
		return gendata_coord_dispatch(solver+6, data);
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
	int64_t err;

	if ((size_t)data % 8 != 0) {
		LOG("nissy_checkdata: buffere is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	for (buf = (char *)data;
	     (err = readtableinfo(data_size, buf, &info)) == NISSY_OK;
	     buf += info.next, data_size -= info.next)
	{
		if (!checkdata(buf, &info)) {
			LOG("Error: data for solver '%s' is corrupted!\n",
			    info.solver);
			return NISSY_ERROR_DATA;
		}

		if (info.next == 0)
			break;
	}

	return err;
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
	int threads,
	unsigned long long data_size,
	const char data[data_size],
	unsigned sols_size,
	char sols[sols_size],
	long long stats[static NISSY_SIZE_SOLVE_STATS]
)
{
	cube_t c;
	long long parse_ret;
	uint8_t h, k;
	int t, opt;

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

/* TODO: checks for minmoves, maxmoves, nissflag */

	if (maxsols == 0) {
		LOG("solve: 'maxsols' is 0, returning no solution\n");
		return 0;
	}

	opt = optimal < 0 ? MAXLEN : optimal;

	t = threads == 0 ? THREADS : threads;
	if (t < 0) {
		LOG("solve: 'threads' is negative. Please provide a "
		    "number of threads between 1 and %d\n", THREADS);
		return NISSY_ERROR_OPTIONS;
	} 
	if (t > THREADS) {
		LOG("solve: 'threads' is above the maximum value of %d\n",
		    THREADS);
		return NISSY_ERROR_OPTIONS;
	}

	if ((size_t)data % 8 != 0) {
		LOG("nissy_solve: buffere is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	if (!strncmp(solver, "h48", 3)) {
		parse_ret = parse_h48_solver(solver, &h, &k);
		if (parse_ret != NISSY_OK)
			return parse_ret;
		return solve_h48(c, minmoves, maxmoves, maxsols,
		    opt, t, data_size, data, sols_size, sols, stats);
	} else if (!strncmp(solver, "coord_", 6)) {
		return solve_coord_dispatch(c, solver + 6, nissflag,
		    minmoves, maxmoves, maxsols, opt, t, data_size, data,
		    sols_size, sols);
	} else {
		LOG("solve: unknown solver '%s'\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
}

long long
nissy_countmoves(
	const char *moves
)
{
	if (moves == NULL)
		return NISSY_ERROR_NULL_POINTER;

	return readmoves(moves, INT_MAX, NULL);
}

long long
nissy_setlogger(
	void (*log)(const char *, ...)
)
{
	nissy_log = log;
	return NISSY_OK;
}
