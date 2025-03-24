/*
Input format for appendsolution tests:

moves on normal
moves on inverse (without parentheses)
unniss flag (0=false, 1=true)
number of transformations
transformations, one per line

See below for the output format.
*/

#include "../test.h"

uint8_t readtrans(const char [NISSY_SIZE_TRANSFORMATION]);
int64_t readmoves(const char *, size_t n, uint8_t [n]);
void solution_moves_reset(solution_moves_t [static 1]);
bool solution_list_init(solution_list_t [static 1], size_t n, char [n]);
int64_t appendsolution(const solution_moves_t [static 1],
    const solution_settings_t [static 1], solution_list_t [static 1]);

void run(void) {
	int i, ntrans;
	char str[STRLENMAX], buf[STRLENMAX];
	solution_moves_t moves;
	solution_settings_t settings;
	solution_list_t list;

	solution_moves_reset(&moves);
	solution_list_init(&list, STRLENMAX, buf);
	settings = (solution_settings_t) {
		.tmask = UINT64_C(0),
		.unniss = false,
		.maxmoves = 20,
		.maxsolutions = 100,
		.optimal = -1,
	};

	fgets(str, STRLENMAX, stdin);
	moves.nmoves = (uint8_t)readmoves(str, 20, moves.moves);
	fgets(str, STRLENMAX, stdin);
	moves.npremoves = (uint8_t)readmoves(str, 20, moves.premoves);
	fgets(str, STRLENMAX, stdin);
	settings.unniss = (bool)atoi(str);
	fgets(str, STRLENMAX, stdin);
	ntrans = atoi(str);
	for (i = 0; i < ntrans; i++) {
		fgets(str, STRLENMAX, stdin);
		settings.tmask |= UINT64_C(1) << (uint64_t)readtrans(str);
	}

	appendsolution(&moves, &settings, &list);

	printf("%s", list.buf);
	printf("Number of solutions: %" PRIu64 "\n", list.nsols);
	printf("Shortest solution length: %" PRIu8 "\n", list.shortest_sol);
	printf("Used bytes: %zu\n", list.used);
}
