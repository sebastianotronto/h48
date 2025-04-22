#include "../test.h"

#define MAXMOVES 150

cube_t applytrans(cube_t, const char *);
uint8_t transform_move(uint8_t, uint8_t);
int64_t readmoves(const char *, size_t n, uint8_t [n]);
cube_t move(cube_t, uint8_t);
cube_t applymoves(cube_t, const char *);
uint8_t readtrans(const char[static NISSY_SIZE_TRANSFORMATION]);

void run(void) {
	char movestr[STRLENMAX], transtr[STRLENMAX], cubestr[STRLENMAX];
	uint8_t t, moves[MAXMOVES];
	int i, n;
	cube_t cube;

	fgets(transtr, STRLENMAX, stdin);
	fgets(movestr, STRLENMAX, stdin);

	cube = solvedcube();
	n = readmoves(movestr, MAXMOVES, moves);
	t = readtrans(transtr);
	for (i = 0; i < n; i++)
		cube = move(cube, transform_move(moves[i], t));

	writecube(cube, STRLENMAX, cubestr);
	printf("%s\n", cubestr);
}
