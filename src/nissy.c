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
STATIC bool checkdata(const unsigned char *, const tableinfo_t [static 1]);
STATIC bool distribution_equal(const uint64_t [static INFO_DISTRIBUTION_LEN],
    const uint64_t [static INFO_DISTRIBUTION_LEN], uint8_t);
STATIC long long write_result(oriented_cube_t, char [static NISSY_SIZE_CUBE]);
STATIC size_t my_strnlen(const char *, size_t); 
STATIC long long nissy_dataid(const char *, char [static NISSY_SIZE_DATAID]);
STATIC long long nissy_gendata_unsafe(
    const char *, unsigned long long, unsigned char *);

#define GETCUBE_OPTIONS(S, F) { .option = S, .fix = F }
struct {
	char *option;
	void (*fix)(long long *, long long *,
	    long long *, long long *, long long *);
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
	LOG("Error parsing H48 solver: must be in \"h48h*k*\" format,"
	    " but got %s\n", fullbuf);
	return NISSY_ERROR_INVALID_SOLVER;
}

STATIC bool
checkdata(const unsigned char *buf, const tableinfo_t info[static 1])
{
	uint64_t distr[INFO_DISTRIBUTION_LEN];

	if (my_strnlen(info->solver, INFO_SOLVER_STRLEN)
	    == INFO_SOLVER_STRLEN) {
		LOG("[checkdata] Error reading table info\n");
		return false;
	} else if (!strncmp(info->solver, "cocsep", 6)) {
		getdistribution_cocsep(
		    (uint32_t *)((char *)buf + INFOSIZE), distr);
	} else if (!strncmp(info->solver, "h48", 3)) {
		getdistribution_h48(buf + INFOSIZE, distr,
		    info->h48h, info->bits);
	} else if (!strncmp(info->solver, "coordinate solver for ", 22)) {
		getdistribution_coord(buf + INFOSIZE,
		    info->solver + 22, distr);
	} else if (!strncmp(info->solver, "eoesep data for h48", 19)) {
		return true;
	} else if (!strncmp(info->solver, "coord helper table for ", 23)) {
		return true;
	} else {
		LOG("[checkdata] unknown solver %s\n", info->solver);
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

	for (i = 0, wrong = 0; i <= MIN(maxvalue, 20); i++) {
		if (expected[i] != actual[i]) {
			wrong++;
			LOG("Value %" PRIu8 ": expected %" PRIu64 ", found %"
			    PRIu64 "\n", i, expected[i], actual[i]);
		}
	}

	return wrong == 0;
}

STATIC long long
write_result(oriented_cube_t cube, char result[static NISSY_SIZE_CUBE])
{
	writecube(cube, NISSY_SIZE_CUBE, result);

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
nissy_inverse(
	const char cube[static NISSY_SIZE_CUBE],
	char result[static NISSY_SIZE_CUBE]
)
{
	oriented_cube_t c, res;
	long long err;

	c = readcube(cube);

	if (iserror(c)) {
		LOG("[inverse] Error: the given cube is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_inverse_error;
	}

	res = (oriented_cube_t) {
		.cube = inverse(c.cube),
		.orientation = c.orientation
	};

	if (!isconsistent(res)) {
		LOG("[inverse] Unknown error: inverted cube is invalid\n");
		err = NISSY_ERROR_UNKNOWN;
		goto nissy_inverse_error;
	}

	return write_result(res, result);

nissy_inverse_error:
	writecube(ZERO_ORIENTED_CUBE, NISSY_SIZE_CUBE, result);
	return err;
}

long long
nissy_applymoves(
	const char cube[static NISSY_SIZE_CUBE],
	const char *moves,
	char result[static NISSY_SIZE_CUBE]
)
{
	oriented_cube_t c, res;
	long long err;

	if (moves == NULL) {
		LOG("[applymoves] Error: 'moves' argument is NULL\n");
		err = NISSY_ERROR_NULL_POINTER;
		goto nissy_applymoves_error;
	}

	c = readcube(cube);

	if (!isconsistent(c)) {
		LOG("[applymoves] Error: given cube is invalid\n");
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
	writecube(ZERO_ORIENTED_CUBE, NISSY_SIZE_CUBE, result);
	return err;
}

long long
nissy_applytrans(
	const char cube[static NISSY_SIZE_CUBE],
	const char transformation[static NISSY_SIZE_TRANSFORMATION],
	char result[static NISSY_SIZE_CUBE]
)
{
	oriented_cube_t c, res;
	long long err;

	c = readcube(cube);

	if (!isconsistent(c)) {
		LOG("[applytrans] Error: given cube is invalid\n");
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
	writecube(ZERO_ORIENTED_CUBE, NISSY_SIZE_CUBE, result);
	return err;
}

long long
nissy_getcube(
	long long ep,
	long long eo,
	long long cp,
	long long co,
	long long orient,
	const char *options,
	char result[static NISSY_SIZE_CUBE]
)
{
	int i;
	oriented_cube_t oc;

	if (options == NULL) {
		LOG("[getcube] Error: 'options' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	for (i = 0; getcube_options[i].option != NULL; i++)
		if (!strcmp(options, getcube_options[i].option))
			getcube_options[i].fix(&ep, &eo, &cp, &co, &orient);

	oc.cube = getcube(ep, eo, cp, co);
	oc.orientation = orient;

	if (!isconsistent(oc)) {
		LOG("[getcube] Error: could not get cube with ep=%lld, "
		    "eo=%lld, cp=%lld, co=%lld, orient=%lld.\n",
		    ep, eo, cp, co, orient);
		return NISSY_ERROR_OPTIONS;
	}

	return write_result(oc, result);
}

long long
nissy_datainfo(
        uint64_t data_size,
	const unsigned char data[data_size]
)
{
	uint8_t i;
	tableinfo_t info;
	long long ret;

	if ((size_t)data % 8 != 0) {
		LOG("[datainfo] Error: buffer is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	ret = readtableinfo(data_size, data, &info);
	if (ret != 0)
		return ret;

	LOG("\n---------\n\n"
	    "Table information for '%s'\n\n"
	    "Size:      %" PRIu64 " bytes\n"
	    "Entries:   %" PRIu64 " (%" PRIu8 " bits per entry)\n",
	    info.solver, info.fullsize, info.entries, info.bits);

	switch (info.type) {
	case TABLETYPE_PRUNING:
		LOG("\nTable distribution:\nValue\tPositions\n");
		for (i = 0; i <= info.maxvalue; i++) {
			LOG("%" PRIu8 "\t%" PRIu64 "\n",
			    i + info.base, info.distribution[i]);
		}
		break;
	case TABLETYPE_SPECIAL:
		LOG("This is an ad-hoc table\n");
		break;
	default:
		LOG("datainfo: unknown table type\n");
		return NISSY_ERROR_DATA;
	}

	if (info.next != 0)
		return nissy_datainfo(data_size - info.next, data + info.next);

	LOG("\n---------\n");

	return NISSY_OK;
}

STATIC long long
nissy_dataid(const char *solver, char dataid[static NISSY_SIZE_DATAID])
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
		LOG("[gendata] Unknown solver %s\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
}

long long
nissy_solverinfo(
	const char *solver,
	char dataid[static NISSY_SIZE_DATAID]
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
	unsigned char data[data_size]
)
{
	return nissy_gendata_unsafe(solver, data_size, data);
}

STATIC long long
nissy_gendata_unsafe(
	const char *solver,
	unsigned long long data_size,
	unsigned char *data
)
{
	long long parse_ret;
	gendata_h48_arg_t arg;

	if (solver == NULL) {
		LOG("[gendata] Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	if ((size_t)data % 8 != 0) {
		LOG("[gendata] Error: buffer is not 8-byte aligned\n");
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
		LOG("[gendata] Unknown solver %s\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
}

long long
nissy_checkdata(
	unsigned long long data_size,
	const unsigned char data[data_size]
)
{
	tableinfo_t info;
	int64_t err;

	if ((size_t)data % 8 != 0) {
		LOG("[checkdata] Error: buffer is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	for (const unsigned char *buf = data;
	     (err = readtableinfo(data_size, buf, &info)) == NISSY_OK;
	     buf += info.next, data_size -= info.next)
	{
		if (!checkdata(buf, &info)) {
			LOG("[checkdata] Error: data for solver '%s' is "
			    "corrupted!\n", info.solver);
			return NISSY_ERROR_DATA;
		}

		if (info.next == 0)
			break;
	}

	return err;
}

long long
nissy_solve(
	const char cube[static NISSY_SIZE_CUBE],
	const char *solver, 
	unsigned nissflag,
	unsigned minmoves,
	unsigned maxmoves,
	unsigned maxsols,
	unsigned optimal,
	unsigned threads,
	unsigned long long data_size,
	const unsigned char data[data_size],
	unsigned sols_size,
	char sols[sols_size],
	long long stats[static NISSY_SIZE_SOLVE_STATS],
	int (*poll_status)(void *),
	void *poll_status_data
)
{
	oriented_cube_t oc;
	long long parse_ret;
	uint8_t h, k;
	int t;

	if (solver == NULL) {
		LOG("[solve] Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	oc = readcube(cube);

	if (!isconsistent(oc)) {
		LOG("[solve] Error: cube is invalid\n");
		return NISSY_ERROR_INVALID_CUBE;
	}

	if (maxmoves > 20) {
		LOG("[solve] 'maxmoves' larger than 20 not supported yet, "
		    "setting it to 20\n");
		maxmoves = 20;
	}

	if (minmoves > maxmoves) {
		LOG("[solve] value provided for 'minmoves' (%u) is larger "
		    "than that provided for 'maxmoves' (%u), setting "
		    "'minmoves' to %u\n", minmoves, maxmoves, maxmoves);
		minmoves = maxmoves;
	}

	if (maxsols == 0) {
		LOG("[solve] 'maxsols' is 0, returning no solution\n");
		return 0;
	}

	if (threads > THREADS)
		LOG("[solve] Selected number of threads (%u) is above the "
		    "maximum value (%d), using %d threads instead\n",
		    threads, THREADS, THREADS);
	t = threads == 0 ? THREADS : MIN(THREADS, threads);

	if ((size_t)data % 8 != 0) {
		LOG("[solve] Error: data buffer is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	if (!strncmp(solver, "h48", 3)) {
		parse_ret = parse_h48_solver(solver, &h, &k);
		if (parse_ret != NISSY_OK)
			return parse_ret;
		return solve_h48(oc, minmoves, maxmoves, maxsols,
		    optimal, t, data_size, data, sols_size, sols, stats,
		    poll_status, poll_status_data);
	} else if (!strncmp(solver, "coord_", 6)) {
		return solve_coord_dispatch(oc, solver + 6, nissflag,
		    minmoves, maxmoves, maxsols, optimal, t, data_size, data,
		    sols_size, sols, poll_status, poll_status_data);
	} else {
		LOG("[solve] Error: unknown solver '%s'\n", solver);
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

	return countmoves(moves);
}

long long
nissy_setlogger(
	void (*log)(const char *, void *),
	void *user_data
)
{
	nissy_log = log;
	nissy_log_data = user_data;
	return NISSY_OK;
}
