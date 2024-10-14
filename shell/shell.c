#include <inttypes.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../src/nissy.h"

#define PRINTCUBE_BUFFER_SIZE UINT64_C(1024)
#define SOLUTIONS_BUFFER_SIZE UINT64_C(500000)
#define MAX_PATH_LENGTH       UINT64_C(10000)

#define FLAG_CUBE         "-cube"
#define FLAG_PERM         "-perm"
#define FLAG_COMMAND      "-command"
#define FLAG_STR_CUBE     "-cubestr"
#define FLAG_FORMAT       "-format"
#define FLAG_FORMAT_IN    "-fin"
#define FLAG_FORMAT_OUT   "-fout"
#define FLAG_MOVES        "-moves"
#define FLAG_TRANS        "-trans"
#define FLAG_SOLVER       "-solver"
#define FLAG_NISSTYPE     "-nisstype"
#define FLAG_MINMOVES     "-m"
#define FLAG_MAXMOVES     "-M"
#define FLAG_OPTIMAL      "-O"
#define FLAG_MAXSOLUTIONS "-n"

#define INFO_CUBEFORMAT(cube) cube " must be given in B32 format."
#define INFO_MOVESFORMAT "The accepted moves are U, D, R, L, F and B, " \
    "optionally followed by a 2, a ' or a 3."
#define INFO_TRANSFORMAT "The transformation must be given in the format " \
    "(rotation|mirrored) (2 letters), for exmple " \
    "'rotation UF' or 'mirrored BL'."
#define INFO_FORMATS "The available formats are H48, B32 and SRC."

typedef struct {
	int command_index;
	char cube[22];
	char cube_perm[22];
	char *str_command;
	char *str_cube;
	char *str_format;
	char *str_format_in;
	char *str_format_out;
	char *str_moves;
	char *str_trans;
	char *str_solver;
	char *str_nisstype;
	unsigned minmoves;
	unsigned maxmoves;
	unsigned optimal;
	unsigned maxsolutions;
} args_t;

static int64_t compose_exec(args_t *);
static int64_t inverse_exec(args_t *);
static int64_t applymoves_exec(args_t *);
static int64_t applytrans_exec(args_t *);
static int64_t frommoves_exec(args_t *);
static int64_t convert_exec(args_t *);
static int64_t randomcube_exec(args_t *);
static int64_t datasize_exec(args_t *);
static int64_t gendata_exec(args_t *);
static int64_t solve_exec(args_t *);
static int64_t solve_scramble_exec(args_t *);
static int64_t help_exec(args_t *);

static int parse_args(int, char **, args_t *);
static bool parse_uint(char *, unsigned *);

static bool set_cube(int, char **, args_t *);
static bool set_cube_perm(int, char **, args_t *);
static bool set_str_command(int, char **, args_t *);
static bool set_str_cube(int, char **, args_t *);
static bool set_str_format(int, char **, args_t *);
static bool set_str_format_in(int, char **, args_t *);
static bool set_str_format_out(int, char **, args_t *);
static bool set_str_moves(int, char **, args_t *);
static bool set_str_trans(int, char **, args_t *);
static bool set_str_solver(int, char **, args_t *);
static bool set_str_nisstype(int, char **, args_t *);
static bool set_minmoves(int, char **, args_t *);
static bool set_maxmoves(int, char **, args_t *);
static bool set_optimal(int, char **, args_t *);
static bool set_maxsolutions(int, char **, args_t *);
static bool set_id(int, char **, args_t *);

static uint64_t rand64(void);

#define OPTION(N, A, S) { .name = N, .nargs = A, .set = S }
struct {
	char *name;
	int nargs;
	bool (*set)(int, char **, args_t *);
} options[] = {
	OPTION(FLAG_CUBE, 1, set_cube),
	OPTION(FLAG_PERM, 1, set_cube_perm),
	OPTION(FLAG_COMMAND, 1, set_str_command),
	OPTION(FLAG_STR_CUBE, 1, set_str_cube),
	OPTION(FLAG_FORMAT, 1, set_str_format),
	OPTION(FLAG_FORMAT_IN, 1, set_str_format_in),
	OPTION(FLAG_FORMAT_OUT, 1, set_str_format_out),
	OPTION(FLAG_MOVES, 1, set_str_moves),
	OPTION(FLAG_TRANS, 1, set_str_trans),
	OPTION(FLAG_SOLVER, 1, set_str_solver),
	OPTION(FLAG_NISSTYPE, 1, set_str_nisstype), /* TODO: more args ? */
	OPTION(FLAG_MINMOVES, 1, set_minmoves),
	OPTION(FLAG_MAXMOVES, 1, set_maxmoves),
	OPTION(FLAG_OPTIMAL, 1, set_optimal),
	OPTION(FLAG_MAXSOLUTIONS, 1, set_maxsolutions),
	OPTION(NULL, 0, NULL)
};
 
#define COMMAND(N, S, D, E) { .name = N, .syn = S, .desc = D, .exec = E }
struct {
	char *name;
	char *syn;
	char *desc;
	int64_t (*exec)(args_t *);
} commands[] = {
/* TODO: add synopsis and description here */
	COMMAND(
		"compose",
		"compose " FLAG_CUBE " CUBE " FLAG_PERM " PERM",
		"Apply on CUBE the permutation defined by PERM. "
		INFO_CUBEFORMAT("CUBE and PERM"),
		compose_exec
	),
	COMMAND(
		"inverse",
		"inverse " FLAG_CUBE " CUBE ",
		"Compute the inverse of the given CUBE. "
		INFO_CUBEFORMAT("CUBE"),
		inverse_exec
	),
	COMMAND(
		"applymoves",
		"applymoves " FLAG_CUBE " CUBE " FLAG_MOVES " MOVES",
		"Apply the given MOVES to the given CUBE. "
		INFO_CUBEFORMAT("CUBE") " " INFO_MOVESFORMAT,
		applymoves_exec
	),
	COMMAND(
		"applytrans",
		"applytrans " FLAG_CUBE " CUBE " FLAG_TRANS " TRANS",
		"Apply the single transformation TRANS to the given CUBE. "
		INFO_CUBEFORMAT("CUBE") " " INFO_TRANSFORMAT,
		applytrans_exec
	),
	COMMAND(
		"frommoves",
		"frommoves " FLAG_MOVES " MOVES",
		"Return the cube obtained by applying the given MOVES "
		"to a solved cube. " INFO_MOVESFORMAT,
		frommoves_exec
	),
	COMMAND(
		"convert",
		"convert " FLAG_STR_CUBE " CUBESTR "
		FLAG_FORMAT_IN " FORMAT_IN " FLAG_FORMAT_OUT " FORMAT_OUT",
		"Convert the cube described by CUBESTR from FORMAT_IN to "
		"FORMAT_OUT."
		INFO_FORMATS " "
		"CUBESTR must be a valid cube in the FORMAT_IN format.",
		convert_exec
	),
	COMMAND(
		"randomcube",
		"randomcube",
		"Returns a random cube in B32 format.",
		randomcube_exec
	),
	COMMAND(
		"datasize",
		"datasize " FLAG_SOLVER " SOLVER",
		"Return the size in bytes of the data table used by "
		"SOLVER when called with the given OPTIONS.",
		datasize_exec
	),
	COMMAND(
		"gendata",
		"gendata " FLAG_SOLVER " SOLVER",
		"Generate the data table used by "
		"SOLVER when called with the given OPTIONS.",
		gendata_exec
	),
	COMMAND(
		"solve",
		"solve " FLAG_SOLVER " SOLVER"
		"[" FLAG_MINMOVES " n] [" FLAG_MAXMOVES " N] "
		FLAG_CUBE " CUBE",
		"Solve the given CUBE using SOLVER, "
		"using at least n and at most N moves. "
		INFO_CUBEFORMAT("CUBE"),
		solve_exec
	),
	COMMAND(
		"solve_scramble",
		"solve_scramble " FLAG_SOLVER " SOLVER"
		"[" FLAG_MINMOVES " n] [" FLAG_MAXMOVES " N] "
		FLAG_CUBE " CUBE",
		"Solve the given SCRAMBLE using SOLVER, "
		"using at least n and at most N moves. "
		INFO_MOVESFORMAT,
		solve_scramble_exec
	),
	COMMAND(
		"help",
		"help [" FLAG_COMMAND " COMMAND]",
		"If no COMMAND is specified, prints some generic information "
		"and the list of commands. Otherwise it prints detailed "
		"information about the specified COMMAND.",
		help_exec
	),
	COMMAND(NULL, NULL, NULL, NULL)
};

char *tablepaths[] = {
	"tables/",
	"",
	NULL
};

static uint64_t
rand64(void)
{
	uint64_t i, ret;

	for (i = 0, ret = 0; i < 64; i++)
		ret |= (uint64_t)(rand() % 2) << i;

	return ret;
}

static int64_t
compose_exec(args_t *args)
{
	char result[22];
	int64_t ret;

	ret = nissy_compose(args->cube, args->cube_perm, result);
	if (ret == NISSY_OK || ret == NISSY_WARNING_UNSOLVABLE)
		printf("%s\n", result);

	return ret;
}

static int64_t
inverse_exec(args_t *args)
{
	char result[22];
	int64_t ret;

	ret = nissy_inverse(args->cube, result);
	if (ret == NISSY_OK || ret == NISSY_WARNING_UNSOLVABLE)
		printf("%s\n", result);

	return ret;
}

static int64_t
applymoves_exec(args_t *args)
{
	char result[22];
	int64_t ret;

	ret = nissy_applymoves(args->cube, args->str_moves, result);
	if (ret == NISSY_OK || ret == NISSY_WARNING_UNSOLVABLE)
		printf("%s\n", result);

	return ret;
}

static int64_t
applytrans_exec(args_t *args)
{
	char result[22];
	int64_t ret;

	ret = nissy_applytrans(args->cube, args->str_trans, result);
	if (ret == NISSY_OK || ret == NISSY_WARNING_UNSOLVABLE)
		printf("%s\n", result);

	return ret;
}

static int64_t
frommoves_exec(args_t *args)
{
	sprintf(args->cube, NISSY_SOLVED_CUBE);
	return applymoves_exec(args);
}

static int64_t
convert_exec(args_t *args)
{
	char result[PRINTCUBE_BUFFER_SIZE];
	int64_t ret;

	ret = nissy_convert(args->str_format_in, args->str_format_out,
	    args->str_cube, PRINTCUBE_BUFFER_SIZE, result);
	if (ret == NISSY_OK || ret == NISSY_WARNING_UNSOLVABLE)
		printf("%s\n", result);

	return ret;
}

static int64_t
randomcube_exec(args_t *args)
{
	char result[PRINTCUBE_BUFFER_SIZE];
	int64_t ret, ep, eo, cp, co;

	ep = rand64();
	eo = rand64();
	cp = rand64();
	co = rand64();
	ret = nissy_getcube(ep, eo, cp, co, "fix", result);
	if (ret == NISSY_OK || ret == NISSY_WARNING_UNSOLVABLE)
		printf("%s\n", result);

	return ret;
}

static int64_t
datasize_exec(args_t *args)
{
	int64_t ret;

	ret = nissy_datasize(args->str_solver);
	if (ret < 0)
		fprintf(stderr, "Unknown error (make sure solver is valid)\n");
	printf("%" PRId64 "\n", ret);

	return ret;
}

static int64_t
gendata_exec(args_t *args)
{
	int i;
	FILE *file;
	char *buf, path[MAX_PATH_LENGTH];
	int64_t ret, size;
	size_t written;

	/* TODO: should give warning if overwriting existing file */
	for (i = 0; tablepaths[i] != NULL; i++) {
		strcpy(path, tablepaths[i]);
		strcat(path, args->str_solver);
		file = fopen(path, "wb");
		if (file != NULL)
			break;
	}

	if (tablepaths[i] == NULL) {
		fprintf(stderr, "Cannot write data to file\n");
		fclose(file);
		return -2;
	}

	size = nissy_datasize(args->str_solver);

	if (size < 0) {
		fprintf(stderr,
		    "Unknown error in retrieving data size"
		    "(make sure solver is valid)\n");
		fclose(file);
		return -3;
	}

	buf = malloc(size);

	ret = nissy_gendata(args->str_solver, size, buf);
	if (ret < 0) {
		fprintf(stderr, "Unknown error in generating data\n");
		fclose(file);
		free(buf);
		return -4;
	}
	if (ret != size) {
		fprintf(stderr, "Unknown error: unexpected data size "
		    "got %" PRId64 ", expected %" PRId64 ")\n", ret, size);
		fclose(file);
		free(buf);
		return -5;
	}

	written = fwrite(buf, size, 1, file);
	fclose(file);
	free(buf);

	if (written != 1) {
		fprintf(stderr,
		    "Error: data was generated correctly, but could not be "
		    "written to file (generated %" PRId64 " bytes, written "
		    "%zu)\n", size, written);
		return -6;
	}

	fprintf(stderr, "Data written to %s\n", path);

	return 0;
}

static int64_t
solve_exec(args_t *args)
{
	int i;
	uint8_t nissflag;
	FILE *file;
	char *buf, solutions[SOLUTIONS_BUFFER_SIZE], path[MAX_PATH_LENGTH];
	int64_t ret, gendata_ret, size;
	size_t read;

	nissflag = NISSY_NISSFLAG_NORMAL; /* TODO: parse str_nisstype */

	for (i = 0; tablepaths[i] != NULL; i++) {
		strcpy(path, tablepaths[i]);
		strcat(path, args->str_solver);
		file = fopen(path, "rb");
		if (file != NULL)
			break;
	}

	if (tablepaths[i] == NULL) {
		fprintf(stderr,
		    "Cannot read data file, "
		    "generating it (this can take a while)\n");
		gendata_ret = gendata_exec(args);
		if (gendata_ret)
			return gendata_ret;
	}

	/* Ugh, this is not elegant TODO */
	if (file == NULL) {
		for (i = 0; tablepaths[i] != NULL; i++) {
			strcpy(path, tablepaths[i]);
			strcat(path, args->str_solver);
			file = fopen(path, "rb");
			if (file != NULL)
				break;
		}
	}

	if (tablepaths[i] == NULL) {
		fprintf(stderr, "Error: data file not found\n");
		fclose(file);
		return -1;
	}

	size = nissy_datasize(args->str_solver);
	buf = malloc(size);
	read = fread(buf, size, 1, file);
	fclose(file);
	if (read != 1) {
  		fprintf(stderr, "Error reading data from file: "
		    "fread() returned %zu instead of 1 when attempting to"
		    "read %" PRId64 " bytes from file %s\n", read, size, path);
		return -2;
	}

	ret = nissy_solve(
	    args->cube, args->str_solver, nissflag, args->minmoves,
	    args->maxmoves, args->maxsolutions, args->optimal,
	    size, buf, SOLUTIONS_BUFFER_SIZE, solutions);

	free(buf);

	if (ret == NISSY_OK || ret == NISSY_WARNING_UNSOLVABLE)
		fprintf(stderr, "No solutions found\n");
	else
		printf("%s", solutions);

	return 0;
}

static int64_t
solve_scramble_exec(args_t *args)
{
	nissy_applymoves(NISSY_SOLVED_CUBE, args->str_moves, args->cube);

	return solve_exec(args);
}

static int64_t
help_exec(args_t *args)
{
	int i;

	if (args->str_command == NULL || args->str_command[0] == '\0') {
		printf("This is a rudimentary shell for the H48 library.\n");
		printf("Available commands and usage:\n\n");
		for (i = 0; commands[i].name != NULL; i++)
			printf("%-15s%s\n", commands[i].name, commands[i].syn);
		printf("\nUse 'help -command COMMAND' for more information.\n");
	} else {
		for (i = 0; commands[i].name != NULL; i++)
			if (!strcmp(args->str_command, commands[i].name))
				break;
		if (commands[i].name == NULL) {
			printf("Unknown command %s\n", args->str_command);
			return 1;
		}
		printf("Command %s\n\n", commands[i].name);
		printf("Synopsis: %s\n\n", commands[i].syn);
		printf("Description: %s\n", commands[i].desc);
	}

	return 0;
}

static int
parse_args(int argc, char **argv, args_t *args)
{
	int i, j, n;

	*args = (args_t) {
		.command_index = -1,
		.cube = "",
		.cube_perm = "",
		.str_cube = "",
		.str_format = "",
		.str_format_in = "",
		.str_format_out = "",
		.str_moves = "",
		.str_trans = "",
		.str_solver = "",
		.str_nisstype = "",
		.minmoves = 0,
		.maxmoves = 20,
		.optimal = -1,
		.maxsolutions = 1,
	};

	if (argc == 0) {
		printf("No command given\n");
		return 1;
	}

	for (i = 0; commands[i].name != NULL; i++) {
		if (!strcmp(argv[0], commands[i].name)) {
			args->command_index = i;
			break;
		}
	}

	if (commands[i].name == NULL) {
		fprintf(stderr, "Unknown command %s\n", argv[0]);
		return 1;
	}

	for (i = 1; i < argc; i++) {
		for (j = 0; options[j].name != NULL; j++) {
			n = argc - i - 1;
			if (strcmp(argv[i], options[j].name))
				continue;
			if (n < options[j].nargs) {
				fprintf(stderr,
				    "Too few arguments for option %s\n",
				    options[j].name);
				return 1;
			}
			if (!options[j].set(n, argv+i+1, args)) {
				fprintf(stderr,
				    "Error parsing arguments for option %s\n",
				    options[j].name);
				return 1;
			}
			i += options[j].nargs;
			break;
		}
		if (options[j].name == NULL) {
			fprintf(stderr, "Unknown option %s\n", argv[i]);
			return 1;
		}
	}

	return 0;
}

bool
parse_uint(char *argv, unsigned *result)
{
	*result = strtol(argv, NULL, 10);

	/* TODO: figure out how errno works and use it */
	return true;
}

static bool
set_cube(int argc, char **argv, args_t *args)
{
	memcpy(args->cube, argv[0], 22);
	args->cube[21] = 0;

	return true;
}

static bool
set_cube_perm(int argc, char **argv, args_t *args)
{
	memcpy(args->cube_perm, argv[0], 22);
	args->cube_perm[21] = 0;

	return true;
}

static bool
set_str_command(int argc, char **argv, args_t *args)
{
	args->str_command = argv[0];

	return true;
}

static bool
set_str_cube(int argc, char **argv, args_t *args)
{
	args->str_cube = argv[0];

	return true;
}

static bool
set_str_format(int argc, char **argv, args_t *args)
{
	args->str_format = argv[0];

	return true;
}

static bool
set_str_format_in(int argc, char **argv, args_t *args)
{
	args->str_format_in = argv[0];

	return true;
}

static bool
set_str_format_out(int argc, char **argv, args_t *args)
{
	args->str_format_out = argv[0];

	return true;
}

static bool
set_str_moves(int argc, char **argv, args_t *args)
{
	args->str_moves = argv[0];

	return true;
}

static bool
set_str_trans(int argc, char **argv, args_t *args)
{
	args->str_trans = argv[0];

	return true;
}

static bool
set_str_solver(int argc, char **argv, args_t *args)
{
	args->str_solver = argv[0];

	return true;
}

static bool
set_str_nisstype(int argc, char **argv, args_t *args)
{
	args->str_nisstype = argv[0];

	return true;
}

static bool
set_minmoves(int argc, char **argv, args_t *args)
{
	return parse_uint(argv[0], &args->minmoves);
}

static bool
set_maxmoves(int argc, char **argv, args_t *args)
{
	return parse_uint(argv[0], &args->maxmoves);
}

static bool
set_optimal(int argc, char **argv, args_t *args)
{
	return parse_uint(argv[0], &args->optimal);
}

static bool
set_maxsolutions(int argc, char **argv, args_t *args)
{
	return parse_uint(argv[0], &args->maxsolutions);
}

void
log_stderr(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	vfprintf(stderr, str, args);
	va_end(args);
}

int
main(int argc, char **argv)
{
	int parse_error;
	args_t args;

	srand(time(NULL));
	nissy_setlogger(log_stderr);

	parse_error = parse_args(argc-1, argv+1, &args);
	if (parse_error)
		return parse_error;

	return (int)commands[args.command_index].exec(&args);
}
