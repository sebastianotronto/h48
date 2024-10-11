/*
This is libnissy (temporarily also known as h48), a Rubik's cube library.

If you include this file, you should also use the following includes:

#include <inttypes>
#include <stdarg>
#include <stdbool>
#include <string>

All the functions return 0 or a positive integer in case of success and
a negative integer in case of error, unless otherwise specified.
You can see the list of error codes below, or use nissy_explainerror().

Arguments of type char [static 22] denote a cube in B32 format.
Other available formats are H48 and SRC. See README.md for more info on
these formats.

Accepted moves are U, D, R, L, F and B, optionally followed by a 2,
a ' or a 3.

A transformation must be given in the format
    (rotation|mirrored) (2 letters)
for example 'rotation UF' or 'mirrored BL'.
*/

/* Error codes */
#define NISSY_OK                     INT64_C(0)
#define NISSY_WARNING_UNSOLVABLE     INT64_C(-1)
#define NISSY_WARNING_NULL_CALLBACK  INT64_C(-2)
#define NISSY_ERROR_INVALID_CUBE     INT64_C(-10)
#define NISSY_ERROR_UNSOLVABLE_CUBE  INT64_C(-11)
#define NISSY_ERROR_INVALID_MOVES    INT64_C(-20)
#define NISSY_ERROR_INVALID_TRANS    INT64_C(-30)
#define NISSY_ERROR_INVALID_FORMAT   INT64_C(-40)
#define NISSY_ERROR_INVALID_SOLVER   INT64_C(-50)
#define NISSY_ERROR_NULL_POINTER     INT64_C(-60)
#define NISSY_ERROR_DATA             INT64_C(-70)
#define NISSY_ERROR_OPTIONS          INT64_C(-80)
#define NISSY_ERROR_INVALID_CODE     INT64_C(-90)
#define NISSY_ERROR_UNKNOWN          INT64_C(-999)

/* Flags for NISS options */
#define NISSY_NISSFLAG_NORMAL  UINT8_C(1)
#define NISSY_NISSFLAG_INVERSE UINT8_C(2)
#define NISSY_NISSFLAG_MIXED   UINT8_C(4)
#define NISSY_NISSFLAG_LINEAR  \
    (NISSY_NISSFLAG_NORMAL | NISSY_NISSFLAG_INVERSE)
#define NISSY_NISSFLAG_ALL     \
    (NISSY_NISSFLAG_NORMAL | NISSY_NISSFLAG_INVERSE | NISSY_NISSFLAG_MIXED)

/*
Apply the secod argument as a permutation on the first argument.

Parameters:
   cube        - The first cube, in B32 format.
   permutation - The second cube, in B32 format. This cube is treated as a
                 permutation and "applied" to the first cube.
   result      - The return parameter for the resulting cube, in B32 format.

Return values:
   NISSY_OK                  - The cubes were composed succesfully.
   NISSY_WARNING_UNSOLVABLE  - The resulting cube is not solvable. This is
                               either because at least on of the given cubes
                               was not solvable, or due to an unknown internal
                               error.
   NISSY_ERROR_INVALID_CUBE  - At least one of the given cubes is invalid.
   NISSY_ERROR_UNKNOWN       - An unknown error occurred.
*/
int64_t nissy_compose(
	const char cube[static 22],
	const char permutation[static 22],
	char result[static 22]
);

/*
Compute the inverse of the given cube.

Parameters:
   cube   - The cube to be inverted, in B32 format.
   result - The return parameter for the resulting cube, in B32 format.

Return values:
   NISSY_OK                  - The cube was inverted succesfully.
   NISSY_WARNING_UNSOLVABLE  - The resulting cube is not solvable. This is
                               either because the given cube was not solvable,
                               or due to an unknown internal error.
   NISSY_ERROR_INVALID_CUBE  - The given cube is invalid.
   NISSY_ERROR_UNKNOWN       - An unknown error occurred.
*/
int64_t nissy_inverse(
	const char cube[static 22],
	char result[static 22]
);

/*
Apply the given sequence of moves on the given cube.

Parameters:
   cube   - The cube to move, in B32 format.
   moves  - The moves to apply to the cube.
   result - The return parameter for the resulting cube, in B32 format.

Return values:
   NISSY_OK                  - The moves were applied succesfully.
   NISSY_WARNING_UNSOLVABLE  - The resulting cube is not solvable. This is
                               either because the given cube was not solvable,
                               or due to an unknown internal error.
   NISSY_ERROR_INVALID_CUBE  - The given cube is invalid.
   NISSY_ERROR_INVALID_MOVES - The given moves are invalid.
*/
int64_t nissy_applymoves(
	const char cube[static 22],
	const char *moves,
	char result[static 22]
);

/*
Apply the single given transformation to the given cube.

Parameters:
   cube           - The cube to be transformed, in B32 format.
   transformation - The transformation in (rotation|mirrored) xy format.
   result         - The return parameter for the resulting cube, in B32 format.

Return values:
   NISSY_OK                  - The transformation was performed succesfully.
   NISSY_WARNING_UNSOLVABLE  - The resulting cube is not solvable. This is
                               probably due to an unknown internal error.
   NISSY_ERROR_INVALID_CUBE  - The given cube is invalid.
   NISSY_ERROR_INVALID_TRANS - The given transformation is invalid.
*/
int64_t nissy_applytrans(
	const char cube[static 22],
	const char *transformation,
	char result[static 22]
);

/*
Apply the given moves to the solved cube.

Parameters:
   moves  - The moves to be applied to the solved cube.
   result - Return parameter for the resulting cube, in B32 format.

Return values:
   NISSY_OK                  - The moves were performed succesfully.
   NISSY_WARNING_UNSOLVABLE  - The resulting cube is not solvable. This is
                               probably due to an unknown internal error.
   NISSY_ERROR_INVALID_MOVES - The given moves are invalid.
*/
int64_t nissy_frommoves(
	const char *moves,
	char result[static 22]
);

/*
Convert the given cube between the two given formats.

Parameters:
   format_in  - The input format.
   format_out - The output format.
   string     - The cube, in format_in format.
   result     - Return parameter for the cube in format_out format. Must be
                large enough to contains the cube in this format.

Return values:
   NISSY_OK                   - The conversion was performed succesfully.
   NISSY_ERROR_INVALID_CUBE   - The given cube is invalid.
   NISSY_ERROR_INVALID_FORMAT - At least one of the given formats is invalid.
   NISSY_ERROR_UNKNOWN        - An unknown error occurred.
*/
int64_t nissy_convert(
	const char *format_in,
	const char *format_out,
	const char *cube,
	char *result
);

/*
Get the cube with the given ep, eo, cp and co values. The values must be in the
ranges specified below, but if the option "fix" is given any values outside its
range will be adjusted before using it. The option "fix" also fixes parity and
orientation issues, resulting always in a solvable cube.

Parameters:
   ep      - The edge permutation, 0 <= ep <= 479001600 (12!)
   eo      - The edge orientation, 0 <= eo <= 2047 (2^11)
   cp      - The corner permutation, 0 <= cp <= 40320 (8!)
   co      - The corner orientation, 0 <= co <= 2187 (3^7)
   options - Other options.
   result  - The return parameter for the resulting cube, in B32 format.

Return values:
   NISSY_OK                 - The cube was generated succesfully.
   NISSY_WARNING_UNSOLVABLE - The resulting cube is unsolvable.
   NISSY_ERROR_OPTIONS      - One or more of the given parameters is invalid.
*/
int64_t nissy_getcube(
	int64_t ep,
	int64_t eo,
	int64_t cp,
	int64_t co,
	const char *options,
	char result[static 22]
);

/*
Compute the size of the data generated by nissy_gendata, when called with
the same parameters, or -1 in case of error.

Parameters:
   solver - The name of the solver.

Return values:
   NISSY_ERROR_INVALID_SOLVER - The given solver is not known.
   NISSY_ERROR_UNKNOWN        - An unknown error occurred.
   Any value >= 0             - The size of the data, in bytes.
*/
int64_t nissy_datasize(
	const char *solver
);

/*
Compute the data for the given solver and store it in generated_data.

Parameters:
   solver - The name of the solver.
   data   - The return parameter for the generated data. Must be large enoguh
            to contain the whole data. It is advised to use nissy_datasize to
            check how much memory is needed.

Return values:
   NISSY_ERROR_INVALID_SOLVER - The given solver is not known.
   NISSY_ERROR_UNKNOWN        - An error occurred while generating the data.
   Any value >= 0             - The size of the data, in bytes.
*/
int64_t nissy_gendata(
	const char *solver,
	void *data
);

/*
Print information on a data table via the provided callback writer.

Parameters:
   data  - The data 
   write - A callback writer with the same signature as printf(3).

Return values:
   NISSY_OK         - The data is correct.
   NISSY_ERROR_DATA - The data contains errors.
*/
int64_t nissy_datainfo(
	const void *data,
	void (*write)(const char *, ...)
);

/*
Solve the given cube using the given solver and options.

Parameters:
   cube         - The cube to solver, in B32 format.
   solver       - The name of the solver.
   nissflag     - The flags for NISS (linear, inverse, mixed, or combinations).
   minmoves     - The minimum number of moves for a solution.
   maxmoves     - The maximum number of moves for a solution.
   maxsolutions - The maximum number of solutions.
   optimal      - If set to a non-negative value, the maximum number of moves
                  above the optimal solution length.
   data         - The data for the solver. Can be computed with gendata.
   solutions    - The return parameter for the solutions. Must be large enough
                  to store all found solutions. The solutions are separated by
                  a '\n' (newline) and a '\0' (NULL character) terminates the
                  list.
                  TODO: replace with callback writer.

Return values:
   NISSY_OK                    - Cube solved succesfully.
   NISSY_ERROR_INVALID_CUBE    - The given cube is invalid.
   NISSY_ERROR_UNSOLVABLE_CUBE - The given cube is valid, but not solvable with
                                 the given solver.
   NISSY_ERROR_OPTIONS         - One or more of the given options are invalid.
   NISSY_ERROR_NULL_POINTER    - One of the provided pointers is null.
   NISSY_ERROR_INVALID_SOLVER  - The given solver is not known.
   Any value >= 0              - The number of solutions found.
*/
int64_t nissy_solve(
	const char cube[static 22],
	const char *solver, 
	uint8_t nissflag,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsolutions,
	int8_t optimal,
	const void *data,
	char *solutions
);

/*
Set a global logger function used by this library.

Parameters:
   write - A callback writer with the same signature as printf(3).

Return values:
   NISSY_OK                    - Logger set succesfully.
   NISSY_WARNING_NULL_CALLBACK - The provided callback writer is NULL.
*/
int64_t nissy_setlogger(
	void (*logger_function)(const char *, ...)
);

/*
Print an explanation of the given error code via the provided callback writer.

Parameters:
   error_code - The error code to be explained. It can be any value returned
                by a function in this library, not necessarily an error.
   write      - A callback writer with the same signature as printf(3).
                Must be non-NULL.

Return values:
   NISSY_OK                    - The error code is known.
   NISSY_WARNING_NULL_CALLBACK - The provided callback writer is NULL.
   NISSY_ERROR_INVALID_CODE    - The error code is unknown.
*/
int64_t nissy_explainerror(
	int64_t error_code,
	void (*write)(const char *, ...)
);
