#include "../benchmark.h"
#include "../../src/cube.h"

#define MAXDEPTH 10
#define HVALUE 2
#define OPTIONS "2;10"
#define LONGOPTIONS "h = 2, max depth = 10"

char *buf;

void run(void) {
	uint32_t *buf32;
	int i;
	int64_t s;

	s = nissy_gendata("H48", OPTIONS, buf);

	if (s == -1) {
		printf("Error generating table\n");
	} else {
		printf("Succesfully generated %" PRId64 " bytes. Table:\n", s);
		buf32 = (uint32_t *)&buf[s-sizeof(uint32_t)*(MAXDEPTH+1)];
		for (i = 0; i <= MAXDEPTH; i++)
			printf("%d:\t%" PRId32 "\n", i, buf32[i]);
	}
}

int main() {
	int64_t size;

	size = nissy_datasize("H48", OPTIONS);
	if (size == -1) {
		printf("gendata_h48 benchmark: error in datasize\n");
		return 1;
	}

	buf = malloc(size);

	time_benchmark(run, "gendata_h48 " LONGOPTIONS);

	return 0;
}
