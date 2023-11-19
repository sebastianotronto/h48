#include "../test.h"

bool allowednextmove(uint8_t *, int, uint8_t);

static char *moves[] = {
	"U", "U2", "U'",
	"D", "D2", "D'",
	"R", "R2", "R'",
	"L", "L2", "L'",
	"F", "F2", "F'",
	"B", "B2", "B'",
};

int main(void) {
	char movestr[STRLENMAX];
	uint8_t m[100], next;
	int n, i, j;

	fgets(movestr, STRLENMAX, stdin);
	n = atoi(movestr);

	for (i = 0; i < n; i++) {
		fgets(movestr, STRLENMAX, stdin);
		movestr[strcspn(movestr, "\n")] = 0;
		for (j = 0; j < 18; j++)
			if (!strcmp(movestr, moves[j]))
				m[i] = j;
	}
	fgets(movestr, STRLENMAX, stdin);
	movestr[strcspn(movestr, "\n")] = 0;
	for (j = 0; j < 18; j++)
		if (!strcmp(movestr, moves[j]))
			next = j;

	fprintf(stderr, "Last two: %s, %s\nNext: %s\n",
	    n > 1 ? moves[m[n-2]] : "-",
	    n > 0 ? moves[m[n-1]] : "-",
	    moves[next]);
	printf("%s\n", allowednextmove(m, n, next) ? "true" : "false");

	return 0;
}
