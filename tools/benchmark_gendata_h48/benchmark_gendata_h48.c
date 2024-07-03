#include "../timerun.h"
#include "../../src/cube.h"

#define MAXDEPTH 10
#define HVALUE 2
#define OPTIONS "2;10"
#define LONGOPTIONS "h = 2, max depth = 10"

#define COCSEPSIZE 1119792
#define ETABLESIZE(h) (((3393 * 495 * 70) >> 1) << (size_t)(h))

char *buf;

void run(void) {
	uint32_t *h48info;
	int i;
	int64_t s;

	s = nissy_gendata("H48", OPTIONS, buf);

	if (s == -1) {
		printf("Error generating table\n");
	} else {
		printf("Succesfully generated %" PRId64 " bytes. Table:\n", s);
		h48info = (uint32_t *)buf + (ETABLESIZE(HVALUE) + COCSEPSIZE) / 4;
		for (i = 0; i < MAXDEPTH+1; i++)
			printf("%d:\t%" PRIu32 "\n", i, h48info[i+1]);
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

	timerun(run, "benchmark gendata_h48 " LONGOPTIONS);

	free(buf);

	return 0;
}
