#include "../tool.h"

#define MAXDEPTH 20
#define HVALUE 0
#define OPTIONS "0;2;20"
#define LONGOPTIONS "h = 0, k = 2, max depth = 20"

#define COCSEPSIZE 1119792
#define ETABLESIZE(h) (((3393 * 495 * 70) >> 2) << (size_t)(h))

uint32_t expected[21] = {
	/* Base value is 8 */
	[0] = 5473562,
	[1] = 34776317,
	[2] = 68566704,
	[3] = 8750867,
};

char *buf;

void run(void) {
	uint32_t *h48info, x;
	int i;
	int64_t s;

	s = nissy_gendata("h48", OPTIONS, buf);

	if (s == -1) {
		printf("Error generating table\n");
	} else {
		printf("Succesfully generated %" PRId64 " bytes. Table:\n", s);
		h48info = (uint32_t *)buf + 1 + (ETABLESIZE(HVALUE) + COCSEPSIZE) / 4;
		for (i = 0; i < 4; i++) {
			x = h48info[i+1];
			printf("%d:\t%" PRIu32, i, x);
			if (x != expected[i])
				printf(" <--- Error! Expected: %" PRIu32,
				    expected[i]);
			printf("\n");
		}
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
