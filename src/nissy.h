/*
This is libnissy (temporarily also known as h48), a Rubik's cube library.

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


/* Constants *****************************************************************/

/* Some constants for size for I/O buffers */
#define NISSY_SIZE_B32            22U
#define NISSY_SIZE_H48            88U
#define NISSY_SIZE_TRANSFORMATION 12U
#define NISSY_SIZE_SOLVE_STATS    10U
#define NISSY_DATAID_SIZE         255U

/* Flags for NISS options */
#define NISSY_NISSFLAG_NORMAL  1U
#define NISSY_NISSFLAG_INVERSE 2U
#define NISSY_NISSFLAG_MIXED   4U
#define NISSY_NISSFLAG_LINEAR \
    (NISSY_NISSFLAG_NORMAL | NISSY_NISSFLAG_INVERSE)
#define NISSY_NISSFLAG_ALL \
    (NISSY_NISSFLAG_NORMAL | NISSY_NISSFLAG_INVERSE | NISSY_NISSFLAG_MIXED)

/* The solved cube in B32 format */
#define NISSY_SOLVED_CUBE "ABCDEFGH=ABCDEFGHIJKL"


/* Library functions *********************************************************/

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
long long
nissy_compose(
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
long long
nissy_inverse(
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
long long
nissy_applymoves(
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
long long
nissy_applytrans(
	const char cube[static NISSY_SIZE_B32],
	const char transformation[static NISSY_SIZE_TRANSFORMATION],
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
long long
nissy_convert(
	const char *format_in,
	const char *format_out,
	const char *cube_string,
	unsigned result_size,
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
long long
nissy_getcube(
	long long ep,
	long long eo,
	long long cp,
	long long co,
	const char *options,
	char result[static NISSY_SIZE_B32]
);

/*
Compute the size of the data generated by nissy_gendata when called for
the given solver, and other useful information.

Parameters:
   solver - The name of the solver.
   dataid - An identifier for the data computed for the solver. Different
            solver may use equivalent data. This identifier can be used
            e.g. as a filename or database key to save and retrieve the
            correct data for each solver, without duplication.

Return values:
   NISSY_ERROR_INVALID_SOLVER - The given solver is not known.
   NISSY_ERROR_NULL_POINTER   - The 'solver' argument is null.
   NISSY_ERROR_UNKNOWN        - An unknown error occurred.
   Any value >= 0             - The size of the data, in bytes.
*/
long long
nissy_solverinfo(
	const char *solver,
	char dataid[static NISSY_DATAID_SIZE]
);

/*
Compute the data for the given solver and store it in generated_data.

Parameters:
   solver    - The name of the solver.
   data_size - The size of the data buffer. It is advised to use nissy_datasize
               to check how much memory is needed.
   data      - The return parameter for the generated data.
               This buffer must have 8-byte alignment.

Return values:
   NISSY_ERROR_INVALID_SOLVER - The given solver is not known.
   NISSY_ERROR_NULL_POINTER   - The 'solver' argument is null.
   NISSY_ERROR_UNKNOWN        - An error occurred while generating the data.
   NISSY_ERROR_DATA           - The data buffer is invalid, for example because
                                it is not 8-byte aligned.
   Any value >= 0             - The size of the data, in bytes.
*/
long long
nissy_gendata(
	const char *solver,
	unsigned long long data_size,
	char data[data_size]
);

/*
Check that the data is a valid data table for a solver.

Parameters:
   data_size - The size of the data buffer.
   data      - The data for the solver. Can be computed with gendata.
               This buffer must have 8-byte alignment.

Return values:
   NISSY_OK         - The data is valid.
   NISSY_ERROR_DATA - The data is invalid.
*/
long long
nissy_checkdata(
	unsigned long long data_size,
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
   threads   - The number of threads to use. Must be less than or equalt to
               the value of the compile-time constant THREADS. If set to 0,
               the default value THREADS will be used.
   data_size - The size of the data buffer.
   data      - The data for the solver. Can be computed with gendata.
               This buffer must have 8-byte alignment.
   sols_size - The size of the solutions buffer.
   sols      - The return parameter for the solutions. The solutions are
               separated by a '\n' (newline) and a '\0' (NULL character)
               terminates the list.
   stats     - An array to store some statistics about the solve.

Return values:
   NISSY_OK                    - Cube solved succesfully.
   NISSY_ERROR_INVALID_CUBE    - The given cube is invalid.
   NISSY_ERROR_UNSOLVABLE_CUBE - The given cube is valid, but not solvable with
                                 the given solver.
   NISSY_ERROR_OPTIONS         - One or more of the given options are invalid.
   NISSY_ERROR_INVALID_SOLVER  - The given solver is not known.
   NISSY_ERROR_NULL_POINTER    - The 'solver' argument is null.
   NISSY_ERROR_DATA            - The data buffer is invalid.
   Any value >= 0              - The number of solutions found.
*/
long long
nissy_solve(
	const char cube[static NISSY_SIZE_B32],
	const char *solver, 
	unsigned nissflag,
	unsigned minmoves,
	unsigned maxmoves,
	unsigned maxsolutions,
	int optimal,
	int threads,
	unsigned long long data_size,
	const char data[data_size],
	unsigned sols_size,
	char sols[sols_size],
	long long stats[static NISSY_SIZE_SOLVE_STATS]
);

/*
Parameters:
   moves  - The moves to be counted.

Return values:
   NISSY_ERROR_INVALID_MOVES - The given moves are invalid.
   NISSY_ERROR_NULL_POINTER  - The 'moves' argument is NULL.
   Any value >= 0            - The number of moves.
*/
long long
nissy_countmoves(
	const char *moves
);

/*
Set a global logger function used by this library.

Parameters:
   write - A callback writer with the same signature as printf(3).

Return values:
   NISSY_OK - Logger set succesfully. No warning or error is goind to be given
              if the logger is NULL or invalid.
*/
long long
nissy_setlogger(
	void (*logger_function)(const char *, ...)
);


/* Error codes ***************************************************************/

/*
The value NISSY_OK denotes a success. If returned by solve, it means
that no solution has been found.
*/
#define NISSY_OK 0LL

/*
The value NISSY_WARNING_UNSOLVABLE is a warning. It means that the
operation was completed succesfully, but the resulting cube is in an
unsolvable state. This could be intended, for example if the user has
provided an unsolvable cube as input.
*/
#define NISSY_WARNING_UNSOLVABLE -1LL

/*
The value NISSY_ERROR_INVALID_CUBE means that the provided cube is
invalid. It could be written in an unknown format, or in a format
different from what specified, or simply ill-formed.
*/
#define NISSY_ERROR_INVALID_CUBE -10LL

/*
The value NISSY_ERROR_UNSOLVABLE_CUBE means that the provided cube is
in an unsolvable state.
*/
#define NISSY_ERROR_UNSOLVABLE_CUBE -11LL

/*
The value NISSY_ERROR_INVALID_MOVES means that the given moves are
invalid.
*/
#define NISSY_ERROR_INVALID_MOVES -20LL

/*
The value NISSY_ERROR_INVALID_TRANS means that the given transformation
is invalid.
*/
#define NISSY_ERROR_INVALID_TRANS -30LL

/*
The value NISSY_ERROR_INVALID_FORMAT means that the given format is
not known.
*/
#define NISSY_ERROR_INVALID_FORMAT -40LL

/*
The value NISSY_ERROR_INVALID_SOLVER means that the given solver is
not known.
*/
#define NISSY_ERROR_INVALID_SOLVER -50LL

/*
The value NISSY_ERROR_NULL_POINTER means that one of the provided pointer
arguments is NULL. For example, it may be returned by solve when called
with a solver that requires some pre-computed data, but the provided
data is NULL.
*/
#define NISSY_ERROR_NULL_POINTER -60LL

/*
The value NISSY_ERROR_BUFFER_SIZE means that one of the buffers provided
is too small. For example, it could be too small to hold the result or
too small to hold the data generated by gendata.
*/
#define NISSY_ERROR_BUFFER_SIZE -61LL

/*
The value NISSY_ERROR_DATA means that the provided data is invalid. For
example, it may be returned by solve when called with incompatible solver
and data arguments.
*/
#define NISSY_ERROR_DATA -70LL

/*
The value NISSY_ERROR_OPTIONS means that one or more of the given options
are invalid. For example, it may be returned by solve when called with
a negative maximum number of solutions.
*/
#define NISSY_ERROR_OPTIONS -80LL

/*
The value NISSY_ERROR_UNKNOWN denotes an unexpected error. It probably
means that there some bug in this library.  If you can, report any error
of this kind to sebastiano@tronto.net. Thanks!
*/
#define NISSY_ERROR_UNKNOWN -999LL
