#include "../timerun.h"
#include "../../src/cube.h"

#define MAXDEPTH 20
#define HVALUE 0
#define OPTIONS "0;20"
#define LONGOPTIONS "h = 0, max depth = 20"

#define COCSEPSIZE 1119792
#define ETABLESIZE(h) (((3393 * 495 * 70) >> 1) << (size_t)(h))

char *buf;

void run(void) {
	uint32_t *h48info;
	int i;
	int64_t s;

	s = nissy_gendata("h48", OPTIONS, buf);

	if (s == -1) {
		printf("Error generating table\n");
	} else {
		printf("Succesfully generated %" PRId64 " bytes. Table:\n", s);
		h48info = (uint32_t *)buf + (ETABLESIZE(HVALUE) + COCSEPSIZE) / 4;
		for (i = 0; i < MAXDEPTH+1; i++)
			printf("%d:\t%" PRIu32 "\n", i, h48info[i+1]);
	}
}

int main(void) {
	int64_t size;

	nissy_setlogger(log_stderr);

	size = nissy_datasize("h48", OPTIONS);
	if (size == -1) {
		printf("gendata_h48 benchmark: error in datasize\n");
		return 1;
	}

	buf = malloc(size);

	timerun(run, "benchmark gendata_h48 " LONGOPTIONS);

	free(buf);

	return 0;
}
