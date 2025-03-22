#include "../test.h"

#define MAXMOVES 20

int64_t readmoves(const char *, int, uint8_t *);
void writemoves(size_t n, uint8_t [n], size_t m, char [m]);
void invertmoves(size_t n, const uint8_t [n], uint8_t [n]);

void run(void) {
	char movestr[STRLENMAX], outstr[STRLENMAX];
	uint8_t moves[MAXMOVES], ret[MAXMOVES];
	int c;

	fgets(movestr, STRLENMAX, stdin);
	c = readmoves(movestr, MAXMOVES, moves);

	invertmoves(c, moves, ret);
	writemoves(c, ret, STRLENMAX, outstr);

	printf("%s\n", outstr);
}
