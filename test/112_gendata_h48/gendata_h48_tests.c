#include "../test.h"

#define COCSEPSIZE 1119792
#define ETABLESIZE ((3393 * 495 * 70) >> 1)

int64_t gendata_h48h0k4(void *, uint8_t);

int64_t gendata_h48_fixture(void *buf, uint8_t maxdepth, uint8_t h) {
	if (h == 0)
		return gendata_h48h0k4(buf, maxdepth);
	fprintf(stderr, "Error: gendata h48 for h>0 not implemented yet\n");
	exit(1);
}

void run(void) {
	char str[STRLENMAX];
	uint8_t i, maxdepth, h;
	uint32_t *buf, *h48info;
	size_t result;

	fgets(str, STRLENMAX, stdin);
	maxdepth = atoi(str);
	fgets(str, STRLENMAX, stdin);
	h = atoi(str);

	buf = (uint32_t *)malloc(sizeof(uint32_t) * 60000000);
	result = gendata_h48_fixture(buf, maxdepth, h);
	h48info = buf + (ETABLESIZE + COCSEPSIZE) / 4;

	printf("%zu\n\n", result);

	printf("cocsepdata:\n");
	printf("Classes: %" PRIu32 "\n", buf[COCSEPSIZE/4-12]);
	printf("Max value: %" PRIu32 "\n", buf[COCSEPSIZE/4-11]);
	for (i = 0; i < 10; i++)
		printf("%" PRIu32 ": %" PRIu32 "\n", i, buf[COCSEPSIZE/4-10+i]);

	printf("\nh48:\n");
	for (i = 0; i < maxdepth+1; i++)
		printf("%" PRIu32 ": %" PRIu32 "\n", i, h48info[i+1]);

	free(buf);
}
