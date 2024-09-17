#include "../test.h"

#define INFOSIZE                 512
#define INFO_SOLVER_STRLEN       100
#define INFO_DISTRIBUTION_LEN    21
#define COCSEP_CLASSES           3393

typedef struct {
	char solver[INFO_SOLVER_STRLEN];
	uint64_t type;
	uint64_t infosize;
	uint64_t fullsize;
	uint64_t hash;
	uint64_t entries;
	uint64_t classes; /* Used only by cocsepdata, for now */
	uint8_t h48h;
	uint8_t bits;
	uint8_t base;
	uint8_t maxvalue;
	uint64_t next;
	uint64_t distribution[INFO_DISTRIBUTION_LEN];
} tableinfo_t;

size_t gendata_cocsep(void *, uint64_t *, cube_t *);
bool readtableinfo(const void *, tableinfo_t *);

void run(void) {
	char buf[2000000];
	uint32_t i;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t rep[COCSEP_CLASSES];
	size_t result;
	tableinfo_t info;

	result = gendata_cocsep(buf, selfsim, rep);
	if (!readtableinfo(buf, &info)) {
		printf("Error reading info from table\n");
		return;
	}

	printf("%zu\n", result);
	printf("Classes: %" PRIu64 "\n", info.classes);
	printf("Max value: %" PRIu8 "\n", info.maxvalue);
	for (i = 0; i < 10; i++)
		printf("%" PRIu32 ": %" PRIu64 "\n", i, info.distribution[i]);
}
