#include "../test.h"

#define COCSEP_CLASSES        3393
#define INFOSIZE              512
#define INFO_SOLVER_STRLEN    100
#define INFO_DISTRIBUTION_LEN 21

typedef struct {
	char solver[INFO_SOLVER_STRLEN];
	uint64_t type;
	uint64_t infosize;
	uint64_t fullsize;
	uint64_t hash;
	uint64_t entries;
	uint64_t classes; /* Used only by cocsepdata, for now */
	uint8_t bits;
	uint8_t base;
	uint8_t maxvalue;
	uint64_t next;
	uint64_t distribution[INFO_DISTRIBUTION_LEN];
} tableinfo_t;

typedef struct {
	uint8_t h;
	uint8_t k;
	uint8_t maxdepth;
	tableinfo_t info;
	void *buf;
	void *h48buf;
	uint32_t *cocsepdata;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t crep[COCSEP_CLASSES];
} gendata_h48_arg_t;

int64_t gendata_h48(gendata_h48_arg_t *);
bool readtableinfo(const void *, tableinfo_t *);

void run(void) {
	char str[STRLENMAX];
	uint8_t i;
	gendata_h48_arg_t arg;
	size_t result, sz;
	tableinfo_t cinfo, hinfo;
	void *h48buf;

	fgets(str, STRLENMAX, stdin);
	arg.maxdepth = atoi(str);
	fgets(str, STRLENMAX, stdin);
	arg.h = atoi(str);
	arg.k = 4;

	sz = gendata_h48(&arg); /* With buf = NULL returns data size */
	arg.buf = malloc(sz);

	result = gendata_h48(&arg);

	if (!readtableinfo(arg.buf, &cinfo)) {
		printf("Error reading cocsep info\n");
		goto end;
	}

	h48buf = (char *)arg.buf + cinfo.next;
	if (!readtableinfo(h48buf, &hinfo)) {
		printf("Error reading h48 info\n");
		goto end;
	}

	printf("%zu\n\n", result);

	printf("cocsepdata:\n");
	printf("Classes: %" PRIu64 "\n", cinfo.classes);
	printf("Max value: %" PRIu8 "\n", cinfo.maxvalue);
	for (i = 0; i <= cinfo.maxvalue; i++)
		printf("%" PRIu32 ": %" PRIu64 "\n", i, cinfo.distribution[i]);

	printf("\nh48:\n");
	for (i = 0; i <= hinfo.maxvalue; i++)
		printf("%" PRIu32 ": %" PRIu64 "\n", i, hinfo.distribution[i]);

end:
	free(arg.buf);
}
