#include "../test.h"

extern char *movestr[];

int64_t readmoves(const char *, size_t n, uint8_t [n]);
uint8_t inverse_move(uint8_t);

void run(void) {
	uint8_t moves[2];

	char moves_string[STRLENMAX];

	fgets(moves_string, STRLENMAX, stdin);
	if (readmoves(moves_string, 2, moves) != 1) {
		printf("Test error: cannot read moves\n");
		return;
	}

	printf("%s\n", movestr[inverse_move(moves[0])]);
}
