#include "../test.h"

#define H 0
#define SIZE (120000000 << (H))

uint32_t buf[SIZE];

size_t gendata_h48(void *, uint8_t);

int main(void) {
	size_t result;

	result = gendata_h48(buf, H);

	printf("nothing\n");

	return 0;
}
