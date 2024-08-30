#include "../test.h"

uint32_t allowednextmoveH48(uint8_t *, uint8_t, uint32_t);

static char *moves[] = {
	"U", "U2", "U'",
	"D", "D2", "D'",
	"R", "R2", "R'",
	"L", "L2", "L'",
	"F", "F2", "F'",
	"B", "B2", "B'",
};

void run(void) {
	char movestr[STRLENMAX];
	uint8_t m[100];
	int n, i, j, bound;

	fgets(movestr, STRLENMAX, stdin);
	bound = atoi(movestr);

	fgets(movestr, STRLENMAX, stdin);
	n = atoi(movestr);

	for (i = 0; i < n; i++) {
		fgets(movestr, STRLENMAX, stdin);
		movestr[strcspn(movestr, "\n")] = 0;
		for (j = 0; j < 18; j++)
			if (!strcmp(movestr, moves[j]))
				m[i] = j;
	}

	fprintf(stderr, "Last two: %s, %s\n",
	    n > 1 ? moves[m[n-2]] : "-",
	    n > 0 ? moves[m[n-1]] : "-");
	uint32_t allowed = allowednextmoveH48(m, n, bound);
	printf("0x%05X\n", allowed);
}
