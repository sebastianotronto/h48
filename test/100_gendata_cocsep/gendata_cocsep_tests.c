#include "../test.h"

size_t gendata_cocsep(void *);

int main(void) {
	uint32_t buf[300000];
	size_t result;

	result = gendata_cocsep(buf);

	printf("%zu\n", result);

	return 0;
}
