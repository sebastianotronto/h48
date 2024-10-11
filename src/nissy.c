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
STATIC bool checkdata(const char *, const tableinfo_t *);

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

STATIC int64_t
write_result(cube_t cube, char result[static 22])
{
	writecube("B32", cube, result);

	if (!issolvable(cube)) {
		LOG("Warning: resulting cube is not solvable\n");
		return NISSY_WARNING_UNSOLVABLE;
	}

	return NISSY_OK;
}

int64_t
nissy_compose(
	const char cube[static 22],
	const char permutation[static 22],
	char result[static 22]
)
{
	cube_t c, p, res;
	int64_t err;

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
	writecube("B32", ZERO_CUBE, result);
	return err;
}

int64_t
nissy_inverse(
	const char cube[static 22],
	char result[static 22]
)
{
	cube_t c, res;
	int64_t err;

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
	writecube("B32", ZERO_CUBE, result);
	return err;
}

int64_t
nissy_applymoves(
	const char cube[static 22],
	const char *moves,
	char result[static 22]
)
{
	cube_t c, res;
	int64_t err;

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
	writecube("B32", ZERO_CUBE, result);
	return err;
}

int64_t
nissy_applytrans(
	const char cube[static 22],
	const char transformation[static 12],
	char result[static 22]
)
{
	cube_t c, res;
	int64_t err;

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
	writecube("B32", ZERO_CUBE, result);
	return err;
}

int64_t
nissy_frommoves(
	const char *moves,
	char result[static 22]
)
{
	cube_t res;
	int64_t err;

	if (moves == NULL) {
		LOG("Error: 'moves' argument is NULL\n");
		err = NISSY_ERROR_NULL_POINTER;
		goto nissy_frommoves_error;
	}

	res = applymoves(SOLVED_CUBE, moves);

	if (!isconsistent(res)) {
		/* Assume we got a reasonable error message from applymoves */
		err = NISSY_ERROR_INVALID_MOVES;
		goto nissy_frommoves_error;
	}

	return write_result(res, result);

nissy_frommoves_error:
	writecube("B32", ZERO_CUBE, result);
	return err;
}

int64_t
nissy_convert(
	const char *format_in,
	const char *format_out,
	const char *cube_string,
        size_t result_size,
	char result[result_size]
)
{
	cube_t c;
	int ret;
	int64_t err;

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

	if (iserror(c)) {
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_convert_error;
	}

	ret = writecube(format_out, c, result);

	if (ret != 0) {
		/* Assume the format was invalid */
		err = NISSY_ERROR_INVALID_FORMAT;
		goto nissy_convert_error;
	}

	if (!isconsistent(c)) {
		err = NISSY_ERROR_UNKNOWN;
		goto nissy_convert_error;
	}

	return NISSY_OK;

nissy_convert_error:
	/* We don't write anything to result, we don't know the format */
	return err;
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

int64_t
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

int64_t
nissy_datainfo(
        size_t data_size,
	const char data[data_size],
	void (*write)(const char *, ...)
)
{
	uint8_t i;
	tableinfo_t info;

	readtableinfo(data, &info);

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

int64_t
nissy_gendata(
	const char *solver,
	size_t data_size,
	char data[data_size]
)
{
	int p;
	gendata_h48_arg_t arg;

	if (solver == NULL) {
		LOG("Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	arg.buf = data;
	if (!strncmp(solver, "h48", 3)) {
		p = parse_h48_solver(solver, &arg.h, &arg.k);
		arg.maxdepth = 20;
		if (p != 0)
			return NISSY_ERROR_UNKNOWN;
		return (int64_t)gendata_h48(&arg);
	} else {
		LOG("gendata: unknown solver %s\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
}

int64_t
nissy_checkdata(
	uint64_t data_size,
	const char data[data_size]
)
{
	char *buf;
	tableinfo_t info;

	for (buf = (char *)data; readtableinfo(buf, &info); buf += info.next) {
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

int64_t
nissy_solve(
	const char cube[static 22],
	const char *solver, 
	uint8_t nissflag,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	size_t data_size,
	const char data[data_size],
	size_t sols_size,
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

	if (minmoves < 0) {
		LOG("solve: 'minmoves' is negative, setting it to 0\n");
		minmoves = 0;
	}

	if (maxmoves < 0) {
		LOG("solve: 'maxmoves' is negative, setting it to 20\n");
		maxmoves = 20;
	}

	if (maxsols < 0) {
		LOG("solve: 'maxsols' is negative, stopping\n");
		return NISSY_ERROR_OPTIONS;
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
				solve_h48_multithread(c, minmoves,
				    maxmoves, maxsols, data, sols) :
				solve_h48(c, minmoves,
				    maxmoves, maxsols, data, sols);
		}
	} else if (!strcmp(solver, "simple")) {
		return solve_simple(
		    c, minmoves, maxmoves, maxsols, optimal, sols);
	} else {
		LOG("solve: unknown solver '%s'\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
}

int64_t
nissy_setlogger(
	void (*log)(const char *, ...)
)
{
	nissy_log = log;

	if (log == NULL)
		return NISSY_WARNING_NULL_CALLBACK;

	return NISSY_OK;
}

int64_t
nissy_explainerror(
	int64_t error_code,
	void (*write)(const char *, ...)
)
{
	if (write == NULL)
		return NISSY_WARNING_NULL_CALLBACK;

	switch (error_code) {
	case NISSY_OK:
		write(
		    "The value %" PRId64 " denotes a success.\n"
		    "If returned by solve, it means that no solutions has "
		    "been found.\n", NISSY_OK
		);
		return NISSY_OK;
	case NISSY_WARNING_UNSOLVABLE:
		write(
		    "The value %" PRId64 " is a warning. It means that the "
		    "operation was completed succesfully, but the resulting "
		    "cube is in an unsolvable state. This could be intended, "
		    "for example if the user has provided an unsolvable cube "
		    "as input.\n", NISSY_WARNING_UNSOLVABLE
		);
		return NISSY_OK;
	case NISSY_WARNING_NULL_CALLBACK:
		write(
		    "The value %" PRId64 " is a warning. It means that the "
		    "provided pointer to a writer function is NULL.\n"
		    "If returned by nissy_setlogger, it means that any future "
		    "log messages will not be printed.\n",
		    NISSY_WARNING_NULL_CALLBACK
		);
		return NISSY_OK;
	case NISSY_ERROR_INVALID_CUBE:
		write(
		    "The value %" PRId64 " means that the provided cube is "
		    "invalid. It could be written in an unknown format, or "
		    "in a format different from what specified, or simply "
		    "ill-formed.\n", NISSY_ERROR_INVALID_CUBE
		);
		return NISSY_OK;
	case NISSY_ERROR_UNSOLVABLE_CUBE:
		write(
		    "The value %" PRId64 " means that the provided cube is "
		    "in an unsolvable state.\n", NISSY_ERROR_INVALID_CUBE
		);
		return NISSY_OK;
	case NISSY_ERROR_INVALID_MOVES:
		write(
		    "The value %" PRId64 " means that the given moves are "
		    "invalid.\n", NISSY_ERROR_INVALID_MOVES
		);
		return NISSY_OK;
	case NISSY_ERROR_INVALID_TRANS:
		write(
		    "The value %" PRId64 " means that the given transformation "
		    "is invalid.\n", NISSY_ERROR_INVALID_TRANS
		);
		return NISSY_OK;
	case NISSY_ERROR_INVALID_FORMAT:
		write(
		    "The value %" PRId64 " means that the given format is not "
		    "known.\n", NISSY_ERROR_INVALID_FORMAT
		);
		return NISSY_OK;
	case NISSY_ERROR_INVALID_SOLVER:
		write(
		    "The value %" PRId64 " means that the given solver is not "
		    "known.\n", NISSY_ERROR_INVALID_SOLVER
		);
		return NISSY_OK;
	case NISSY_ERROR_NULL_POINTER:
		write(
		    "The value %" PRId64 " means that one of the provided "
		    "pointer arguments is NULL. For example, it may be "
		    "returned by solve when called with a solver that "
		    "requires some pre-computed data, but the provided data "
		    "is NULL.\n", NISSY_ERROR_NULL_POINTER
		);
		return NISSY_OK;
	case NISSY_ERROR_DATA:
		write(
		    "The value %" PRId64 " means that the provided data is "
		    "invalid. For example, it may be returned by solve when "
		    "called with incompatible solver and data arguments.\n",
		    NISSY_ERROR_DATA
		);
		return NISSY_OK;
	case NISSY_ERROR_OPTIONS:
		write(
		    "The value %" PRId64 " means that one or more of the "
		    "given options are invalid. For example, it may be "
		    "returned by solve when called with a negative maximum "
		    "number of solutions.\n", NISSY_ERROR_OPTIONS
		);
		return NISSY_OK;
	case NISSY_ERROR_INVALID_CODE:
		write(
		    "The value %" PRId64 " means that the given error code "
		    "is not known. It may be returned by explainerror.\n",
		    NISSY_ERROR_INVALID_CODE
		);
		return NISSY_OK;
	case NISSY_ERROR_UNKNOWN:
		write(
		    "The value %" PRId64 " denotes an unexpected error. It "
		    "probably means that there some bug in this library.\n"
		    "If you can, report any error of this kind to "
		    "sebastiano@tronto.net. Thanks!\n", NISSY_ERROR_UNKNOWN
		);
		return NISSY_OK;
	default:
		break;
	}

	if (error_code > 0) {
		write(
		    "A positive return values does not denote an error\n"
		    "If returned by gendata or datasize, it denotes the size "
		    "of the data, in bytes\n"
		    "If returned by solve, it denotes the number of solutions "
		    "found.\n"
		);
		return NISSY_OK;
	} else {
		write("Unknown error code %" PRId64 "\n", error_code);
		return NISSY_ERROR_INVALID_CODE;
	}
}
