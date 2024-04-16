#include "../test.h"

#define CSIZE 1200000
#define ESIZE 250000000

uint32_t buf_c[CSIZE/4];
uint32_t buf_e[ESIZE/4];

size_t gendata_cocsep(void *);
size_t gendata_eoesep(uint8_t, uint8_t, const void *, void *);

int main(void) {
	uint32_t i;
	size_t result;

	gendata_cocsep(buf_c);
	result = gendata_eoesep(0U, 4U, buf_c, buf_e);

	printf("nothing\n");

	return 0;
}
