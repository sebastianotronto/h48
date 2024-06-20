/* include: inttypes, stdarg, stdbool, string */

/*
All the functions below return 0 in case of success and a positive
number in case of error, unless otherwise specified. See (TODO:
documentation) for details.
*/

int64_t nissy_compose(
	const char cube[static 22],
	const char permutation[static 22],
	char result[static 22]
);

int64_t nissy_inverse(
	const char cube[static 22],
	char result[static 22]
);

int64_t nissy_applymoves(
	const char cube[static 22],
	const char *moves,
	char result[static 22]
);

int64_t nissy_applytrans(
	const char cube[static 22],
	const char *transformation,
	char result[static 22]
);

int64_t nissy_frommoves(
	const char *moves,
	char result[static 22]
);

int64_t nissy_readcube(
	const char *format,
	const char *cube_string,
	char result[static 22]
);

int64_t nissy_writecube(
	const char *format,
	const char cube[static 22],
	char *result
);

int64_t nissy_convertcube(
	const char *format_in,
	const char *format_out,
	const char *cube_string,
	char *result
);

/*
Returns the size of the data generated by nissy_gendata, when called with
the same parameters, or -1 in case of error. The returned value can be
slightly larger than the actual table size.
*/
int64_t nissy_datasize(
	const char *solver,
	const char *options /* TODO: remove options, use only solver name */
);

/* Returns the number of bytes written, or -1 in case of error */
int64_t nissy_gendata(
	const char *solver,
	const char *options, /* TODO: remove options, use only solver name */
	void *generated_data
);

/* Returns the number of solutions found, or -1 in case of error */
int64_t nissy_solve(
	const char cube[static 22],
	const char *solver, 
	const char *options, /* TODO: remove options, use only solver name */
	const char *nisstype, /* TODO: remove, use flags */
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsolutions,
	int8_t optimal,
	const void *data,
	char *solutions
);

void nissy_setlogger(void (*logger_function)(const char *, ...));
