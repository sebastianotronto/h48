/*
This is libnissy (temporarily also known as h48), a Rubik's cube library.

If you include this file, you should also use the following includes:

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

All the functions return 0 or a positive integer in case of success and
a negative integer in case of error, unless otherwise specified. See at
the bottom of this file for the list of error codes and their meaning.

All cube arguments are in B32 formats, unless otherwise specified.
Other available formats are H48 and SRC. See README.md for more info on
these formats.

Accepted moves are U, D, R, L, F and B, optionally followed by a 2,
a ' or a 3.

A transformation must be given in the format
    (rotation|mirrored) (2 letters)
for example 'rotation UF' or 'mirrored BL'.
*/

/* Some constants for size for I/O buffers */
#define NISSY_SIZE_B32            UINT64_C(22)
#define NISSY_SIZE_H48            UINT64_C(88)
#define NISSY_SIZE_TRANSFORMATION UINT64_C(12)

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
	const char cube[static NISSY_SIZE_B32],
	const char permutation[static NISSY_SIZE_B32],
	char result[static NISSY_SIZE_B32]
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
	const char cube[static NISSY_SIZE_B32],
	char result[static NISSY_SIZE_B32]
);

/*
Apply the given sequence of moves on the given cube.

Parameters:
   cube   - The cube to move, in B32 format.
   moves  - The moves to apply to the cube. Must be a NULL-terminated string.
   result - The return parameter for the resulting cube, in B32 format.

Return values:
   NISSY_OK                  - The moves were applied succesfully.
   NISSY_WARNING_UNSOLVABLE  - The resulting cube is not solvable. This is
                               either because the given cube was not solvable,
                               or due to an unknown internal error.
   NISSY_ERROR_INVALID_CUBE  - The given cube is invalid.
   NISSY_ERROR_INVALID_MOVES - The given moves are invalid.
   NISSY_ERROR_NULL_POINTER  - The 'moves' argument is NULL.
*/
int64_t nissy_applymoves(
	const char cube[static NISSY_SIZE_B32],
	const char *moves,
	char result[static NISSY_SIZE_B32]
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
	const char cube[static NISSY_SIZE_B32],
	const char transformation[static NISSY_SIZE_TRANSFORMATION],
	char result[static NISSY_SIZE_B32]
);

/*
Apply the given moves to the solved cube.

Parameters:
   moves  - The moves to be applied to the solved cube. Must be a
            NULL-terminated string.
   result - Return parameter for the resulting cube, in B32 format.

Return values:
   NISSY_OK                  - The moves were performed succesfully.
   NISSY_WARNING_UNSOLVABLE  - The resulting cube is not solvable. This is
                               probably due to an unknown internal error.
   NISSY_ERROR_INVALID_MOVES - The given moves are invalid.
   NISSY_ERROR_NULL_POINTER  - The 'moves' argument is NULL.
*/
int64_t nissy_frommoves(
	const char *moves,
	char result[static NISSY_SIZE_B32]
);

/*
Convert the given cube between the two given formats.

Parameters:
   format_in   - The input format.
   format_out  - The output format.
   cube_string - The cube, in format_in format.
   retult_size - The allocated size of the result array.
   result      - Return parameter for the cube in format_out format.

Return values:
   NISSY_OK                   - The conversion was performed succesfully.
   NISSY_ERROR_BUFFER_SIZE    - The given buffer is too small for the result.
   NISSY_ERROR_INVALID_CUBE   - The given cube is invalid.
   NISSY_ERROR_INVALID_FORMAT - At least one of the given formats is invalid.
   NISSY_ERROR_UNKNOWN        - An unknown error occurred.
   NISSY_ERROR_NULL_POINTER   - At least one of 'format_in', 'format_out' or
                                'cube_string' arguments is NULL.
*/
int64_t nissy_convert(
	const char *format_in,
	const char *format_out,
	const char *cube_string,
	uint64_t result_size,
	char result[result_size]
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
	char result[static NISSY_SIZE_B32]
);

/*
Compute the size of the data generated by nissy_gendata, when called with
the same parameters, or -1 in case of error.

Parameters:
   solver - The name of the solver.

Return values:
   NISSY_ERROR_INVALID_SOLVER - The given solver is not known.
   NISSY_ERROR_NULL_POINTER   - The 'solver' argument is null.
   NISSY_ERROR_UNKNOWN        - An unknown error occurred.
   Any value >= 0             - The size of the data, in bytes.
*/
int64_t nissy_datasize(
	const char *solver
);

/*
Compute the data for the given solver and store it in generated_data.

Parameters:
   solver    - The name of the solver.
   data_size - The size of the data buffer. It is advised to use nissy_datasize
               to check how much memory is needed.
   data      - The return parameter for the generated data.

Return values:
   NISSY_ERROR_INVALID_SOLVER - The given solver is not known.
   NISSY_ERROR_NULL_POINTER   - The 'solver' argument is null.
   NISSY_ERROR_UNKNOWN        - An error occurred while generating the data.
   Any value >= 0             - The size of the data, in bytes.
*/
int64_t nissy_gendata(
	const char *solver,
	uint64_t data_size,
	char data[data_size]
);

/*
Check that the data is a valid data table for a solver.

Parameters:
   data_size - The size of the data buffer.
   data      - The data for the solver. Can be computed with gendata.

Return values:
   NISSY_OK         - The data is valid.
   NISSY_ERROR_DATA - The data is invalid.
*/
int64_t nissy_checkdata(
	uint64_t data_size,
	const char data[data_size]
);

/*
Solve the given cube using the given solver and options.

Parameters:
   cube      - The cube to solver, in B32 format.
   solver    - The name of the solver.
   nissflag  - The flags for NISS (linear, inverse, mixed, or combinations).
   minmoves  - The minimum number of moves for a solution.
   maxmoves  - The maximum number of moves for a solution.
   maxsols   - The maximum number of solutions.
   optimal   - If set to a non-negative value, the maximum number of moves
               above the optimal solution length.
   data_size - The size of the data buffer.
   data      - The data for the solver. Can be computed with gendata.
   sols_size - The size of the solutions buffer.
   sols      - The return parameter for the solutions. The solutions are
               separated by a '\n' (newline) and a '\0' (NULL character)
               terminates the list.

Return values:
   NISSY_OK                    - Cube solved succesfully.
   NISSY_ERROR_INVALID_CUBE    - The given cube is invalid.
   NISSY_ERROR_UNSOLVABLE_CUBE - The given cube is valid, but not solvable with
                                 the given solver.
   NISSY_ERROR_OPTIONS         - One or more of the given options are invalid.
   NISSY_ERROR_INVALID_SOLVER  - The given solver is not known.
   NISSY_ERROR_NULL_POINTER    - The 'solver' argument is null.
   Any value >= 0              - The number of solutions found.
*/
int64_t nissy_solve(
	const char cube[static NISSY_SIZE_B32],
	const char *solver, 
	uint8_t nissflag,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsolutions,
	int8_t optimal,
	uint64_t data_size,
	const char data[data_size],
	uint64_t sols_size,
	char sols[sols_size]
);

/*
Set a global logger function used by this library.

Parameters:
   write - A callback writer with the same signature as printf(3).

Return values:
   NISSY_OK - Logger set succesfully. No warning or error is goind to be given
              if the logger is NULL or invalid.
*/
int64_t nissy_setlogger(
	void (*logger_function)(const char *, ...)
);


/* Error codes */

/*
The value NISSY_OK denotes a success. If returned by solve, it means
that no solution has been found.
*/
#define NISSY_OK                     INT64_C(0)

/*
The value NISSY_WARNING_UNSOLVABLE is a warning. It means that the
operation was completed succesfully, but the resulting cube is in an
unsolvable state. This could be intended, for example if the user has
provided an unsolvable cube as input.
*/
#define NISSY_WARNING_UNSOLVABLE     INT64_C(-1)

/*
The value NISSY_ERROR_INVALID_CUBE means that the provided cube is
invalid. It could be written in an unknown format, or in a format
different from what specified, or simply ill-formed.
*/
#define NISSY_ERROR_INVALID_CUBE     INT64_C(-10)

/*
The value NISSY_ERROR_UNSOLVABLE_CUBE means that the provided cube is
in an unsolvable state.
*/
#define NISSY_ERROR_UNSOLVABLE_CUBE  INT64_C(-11)

/*
The value NISSY_ERROR_INVALID_MOVES means that the given moves are
invalid.
*/
#define NISSY_ERROR_INVALID_MOVES    INT64_C(-20)

/*
The value NISSY_ERROR_INVALID_TRANS means that the given transformation
is invalid.
*/
#define NISSY_ERROR_INVALID_TRANS    INT64_C(-30)

/*
The value NISSY_ERROR_INVALID_FORMAT means that the given format is
not known.
*/
#define NISSY_ERROR_INVALID_FORMAT   INT64_C(-40)

/*
The value NISSY_ERROR_INVALID_SOLVER means that the given solver is
not known.
*/
#define NISSY_ERROR_INVALID_SOLVER   INT64_C(-50)

/*
The value NISSY_ERROR_NULL_POINTER means that one of the provided pointer
arguments is NULL. For example, it may be returned by solve when called
with a solver that requires some pre-computed data, but the provided
data is NULL.
*/
#define NISSY_ERROR_NULL_POINTER     INT64_C(-60)

/*
The value NISSY_ERROR_BUFFER_SIZE means that one of the buffers provided
is too small. For example, it could be too small to hold the result or
too small to hold the data generated by gendata.
*/
#define NISSY_ERROR_BUFFER_SIZE      INT64_C(-61)

/*
The value NISSY_ERROR_DATA means that the provided data is invalid. For
example, it may be returned by solve when called with incompatible solver
and data arguments.
*/
#define NISSY_ERROR_DATA             INT64_C(-70)

/*
The value NISSY_ERROR_OPTIONS means that one or more of the given options
are invalid. For example, it may be returned by solve when called with
a negative maximum number of solutions.
*/
#define NISSY_ERROR_OPTIONS          INT64_C(-80)

/*
The value NISSY_ERROR_UNKNOWN denotes an unexpected error. It probably
means that there some bug in this library.  If you can, report any error
of this kind to sebastiano@tronto.net. Thanks!
*/
#define NISSY_ERROR_UNKNOWN          INT64_C(-999)
