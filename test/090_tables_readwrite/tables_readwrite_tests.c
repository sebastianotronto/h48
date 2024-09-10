#include "../test.h"

#define INFOSIZE                 512
#define INFO_SOLVER_STRLEN       20
#define INFO_DISTRIBUTION_LEN    21

typedef struct {
	char solver[INFO_SOLVER_STRLEN];
	uint64_t type;
	uint64_t infosize;
	uint64_t fullsize;
	uint64_t hash;
	uint64_t entries;
	uint8_t bits;
	uint8_t base;
	uint8_t maxvalue;
	uint64_t next;
	uint64_t distribution[INFO_DISTRIBUTION_LEN];
} tableinfo_t;

bool readtableinfo(const void *, tableinfo_t *);
bool writetableinfo(const tableinfo_t *, void *);

uint64_t readn(void) {
	char str[STRLENMAX];

	fgets(str, STRLENMAX, stdin);
	return atoll(str);
}

tableinfo_t test_readinfo(void) {
	int i;
	tableinfo_t ret;
	char emptyline[2];

	fgets(ret.solver, INFO_SOLVER_STRLEN, stdin);
	for (i = 0; i < INFO_SOLVER_STRLEN; i++)
		if (ret.solver[i] == '\n')
			ret.solver[i] = 0;

	fgets(emptyline, 2, stdin);

	ret.type = readn();
	ret.infosize = readn();
	ret.fullsize = readn();
	ret.hash = readn();
	ret.entries = readn();
	ret.bits = (uint8_t)readn();
	ret.base = (uint8_t)readn();
	ret.maxvalue = (uint8_t)readn();
	ret.next = readn();

	fgets(emptyline, 2, stdin);

	for (i = 0; i < INFO_DISTRIBUTION_LEN; i++)
		ret.distribution[i] = readn();

	return ret;
}

void test_writeinfo(tableinfo_t info) {
	int i;

	printf("%s\n", info.solver);
	printf("\n");

	printf("%" PRIu64 "\n", info.type);
	printf("%" PRIu64 "\n", info.infosize);
	printf("%" PRIu64 "\n", info.fullsize);
	printf("%" PRIu64 "\n", info.hash);
	printf("%" PRIu64 "\n", info.entries);
	printf("%" PRIu8 "\n", info.bits);
	printf("%" PRIu8 "\n", info.base);
	printf("%" PRIu8 "\n", info.maxvalue);
	printf("%" PRIu64 "\n", info.next);
	printf("\n");

	for (i = 0; i < INFO_DISTRIBUTION_LEN; i++)
		printf("%" PRIu64 "\n", info.distribution[i]);
}

void run(void) {
	char buf[INFOSIZE];
	tableinfo_t expected, actual;

	expected = test_readinfo();
	writetableinfo(&expected, buf);
	readtableinfo(buf, &actual);
	test_writeinfo(actual);
}
