int check(cube_t, char [static 22]);

int
check(cube_t cube, char result[static 22])
{
	if (!isconsistent(cube)) {
		writecube("B32", zero, result);
		return 2;
	}

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

	return check(res, result);
}

int
nissy_inverse(
	const char cube[static 22],
	char result[static 22]
);

int nissy_applymoves(
	const char cube[static 22],
	const char *moves,
	char result[static 22]
);

int nissy_applytrans(
	const char cube[static 22],
	const char *transformation,
	char result[static 22]
);

int nissy_frommoves(
	const char *moves,
	char result[static 22]
);

int nissy_readcube(
	const char *format,
	const char *cube_string,
	char result[static 22]
);

int nissy_writecube(
	const char *format,
	const char cube[static 22],
	char *result
);

int64_t nissy_gendata(
	const char *solver,
	const char *options,
	void *data
);

int64_t nissy_solve(
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
);
