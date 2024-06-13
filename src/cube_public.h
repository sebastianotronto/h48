#include "cube.h"

int write_result(cube_t, char [static 22]);

int
write_result(cube_t cube, char result[static 22])
{
	if (!isconsistent(cube)) {
		writecube("B32", zero, result);
		return 2;
	}

	writecube("B32", cube, result);

	return issolvable(cube) ? 0 : 1;
}

int
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

int
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

int
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

int
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

int
nissy_frommoves(
	const char *moves,
	char result[static 22]
)
{
	cube_t res;

	res = applymoves(solved, moves);

	return write_result(res, result);
}

int
nissy_readcube(
	const char *format,
	const char *cube_string,
	char result[static 22]
)
{
	cube_t res;

	res = readcube(format, cube_string);

	return write_result(res, result);
}

int
nissy_convertcube(
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

int
nissy_writecube(
	const char *format,
	const char cube[static 22],
	char *result
)
{
	return nissy_convertcube("B32", format, cube, result);
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
	/* TODO: move gendata here? */
	return gendata(solver, options, data);
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
	/* TODO: move solve_generic here? */
	return -1;
}
