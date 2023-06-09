#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../src/cube.h"

#define STRLENMAX 10000
#define MOVESMAX 1000

int main() {
	char str[STRLENMAX];
	int i, n;
	move_t moves[MOVESMAX];
	cube_t cube;

	fgets(str, STRLENMAX, stdin);
	n = readmoves(str, moves);

	if (n == -1) {
		printf("Error reading moves\n");
		return 1;
	}

	fgets(str, STRLENMAX, stdin);
	cube = readcube(str);

	for (i = 0; i < n; i++)
		cube = move(cube, moves[i]);

	if (cube.e == errorcube.e && cube.c == errorcube.c) {
		printf("Error moving cube\n");
	} else if (!isconsistent(cube)) {
		printf("Moved cube is inconsistent\n");
	} else {
		writecube(cube, str);
		printf("%s\n", str);
	}

	return 0;
}
