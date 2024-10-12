#include "../test.h"

#define MAXMOVES 20

int64_t readmoves(const char *, int, uint8_t *);
void writemoves(uint8_t *, int, uint64_t, char *);
void invertmoves(uint8_t *, uint8_t, uint8_t *);

void run(void) {
	char movestr[STRLENMAX], outstr[STRLENMAX];
	uint8_t moves[MAXMOVES], ret[MAXMOVES];
	int c;

	fgets(movestr, STRLENMAX, stdin);
	c = readmoves(movestr, MAXMOVES, moves);

	invertmoves(moves, c, ret);
	writemoves(ret, c, STRLENMAX, outstr);

	printf("%s\n", outstr);
}
