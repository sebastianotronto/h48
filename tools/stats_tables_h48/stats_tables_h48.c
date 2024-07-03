#include <time.h>
#include "../timerun.h"
#include "../../src/cube.h"

#define MAXMOVES 20
#define NCUBES 1000

typedef struct { uint8_t n[16]; } i128;

char *buf;

i128 rand128(void) {
	uint8_t i, j;
	i128 ret = {0};

	for (i = 0; i < 16; i++)
		for (j = 0; j < 8; j++)
			ret.n[i] |= (uint8_t)(rand() % 2) << j;

	return ret;
}

void output(int64_t v[13][100]) {
/* TODO: write to file and output only cocsepdata table stats */
}

void run(void) {
	uint32_t *h48info;
	int i, j;
	char sols[13], cube[22];
	int64_t s, v[13][100] = {0};

	s = nissy_gendata("H48stats", "", buf);

	if (s == -1) {
		printf("Error generating table\n");
		return;
	}

	for (i = 0; i < NCUBES; i++) {
		nissy_gencube(rand128(), "", cube);
		nissy_solve(cube, "H48stats",
		    "", "", "", 0, MAXMOVES, 1, -1, buf, sols);
		for (j = 0; j < 13; j++)
			v[j][(int)sols[j]]++;
	}

	output(v);
}

int main() {
	int64_t size;

	srand(time(NULL));

	size = nissy_datasize("H48", OPTIONS);
	if (size == -1) {
		printf("h48 stats: error in datasize\n");
		return 1;
	}

	buf = malloc(size);

	timerun(run, "h48 table stats");

	free(buf);

	return 0;
}
