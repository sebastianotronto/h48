#include "../test.h"

uint8_t corner(cube_fast_t, int);
uint8_t edge(cube_fast_t, int);

int main(void) {
	int i;
	char str[STRLENMAX], *aux;
	cube_t cube;
	cube_fast_t fast;

        aux = str;
	while (fgets(aux, STRLENMAX, stdin) != NULL)
		while (*aux != '\n')
			aux++;

	cube = readcube("H48", str);
	fast = cubetofast(cube);

	for (i = 0; i < 8; i++)
		printf("%" PRIu8 " ", corner(fast, i));
	printf("\n");
	for (i = 0; i < 12; i++)
		printf("%" PRIu8 " ", edge(fast, i));
	printf("\n");

	return 0;
}
