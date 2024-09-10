#define OFFSET(B, K)             (((uint8_t *)B) + K)
#define INFOSIZE                 512
#define INFO_OFFSET_SOLVER       0
#define INFO_SOLVER_STRLEN       20
#define INFO_OFFSET_INFOSIZE     INFO_SOLVER_STRLEN
#define INFO_OFFSET_FULLSIZE     (INFO_OFFSET_INFOSIZE + sizeof(uint64_t))
#define INFO_OFFSET_HASH         (INFO_OFFSET_FULLSIZE + sizeof(uint64_t))
#define INFO_OFFSET_ENTRIES      (INFO_OFFSET_HASH + sizeof(uint64_t))
#define INFO_OFFSET_BITS         (INFO_OFFSET_ENTRIES + sizeof(uint64_t))
#define INFO_OFFSET_BASE         (INFO_OFFSET_BITS + sizeof(uint8_t))
#define INFO_OFFSET_MAXVALUE     (INFO_OFFSET_BASE + sizeof(uint8_t))
#define INFO_OFFSET_NEXT         (INFO_OFFSET_MAXVALUE + sizeof(uint8_t))
#define INFO_OFFSET_DISTRIBUTION (INFO_OFFSET_NEXT + sizeof(uint64_t))
#define INFO_DISTRIBUTION_LEN    21

typedef struct {
	char solver[INFO_SOLVER_STRLEN];
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

STATIC bool readtableinfo(const void *, tableinfo_t *);
STATIC bool writetableinfo(const tableinfo_t *, void *);

STATIC bool
readtableinfo(const void *buf, tableinfo_t *info)
{
	if (buf == NULL) {
		LOG("Error reading table: buffer in NULL\n");
		return false;
	}

	if (info == NULL) {
		LOG("Error reading table info: info struct is NULL\n");
		return false;
	}

	memcpy(info->solver, OFFSET(buf, INFO_OFFSET_SOLVER),
	    INFO_SOLVER_STRLEN);
	info->infosize = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_INFOSIZE);
	info->fullsize = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_FULLSIZE);
	info->hash = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_HASH);
	info->entries = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_ENTRIES);
	info->bits = *OFFSET(buf, INFO_OFFSET_BITS);
	info->base = *OFFSET(buf, INFO_OFFSET_BASE);
	info->maxvalue = *OFFSET(buf, INFO_OFFSET_MAXVALUE);
	info->next = *(const uint64_t* )OFFSET(buf, INFO_OFFSET_NEXT);
	memcpy(info->distribution, OFFSET(buf, INFO_OFFSET_DISTRIBUTION),
	    INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	return true;
}

STATIC bool
writetableinfo(const tableinfo_t *info, void *buf)
{
	int i;

	if (buf == NULL) {
		LOG("Error reading table: buffer in NULL\n");
		return false;
	}

	if (info == NULL) {
		LOG("Error writing table info: provided info is NULL\n");
		return false;
	}

	memcpy(OFFSET(buf, INFO_OFFSET_SOLVER), info->solver,
	    INFO_SOLVER_STRLEN);

	/* Zeroing all chars after the end of the string, for consistency */
	for (i = 1; i < INFO_SOLVER_STRLEN; i++)
		if (*OFFSET(buf, i) == 0)
			*OFFSET(buf, i) = 0;

	*(uint64_t *)OFFSET(buf, INFO_OFFSET_INFOSIZE) = info->infosize;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_FULLSIZE) = info->fullsize;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_HASH) = info->hash;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_ENTRIES) = info->entries;
	*OFFSET(buf, INFO_OFFSET_BITS) = info->bits;
	*OFFSET(buf, INFO_OFFSET_BASE) = info->base;
	*OFFSET(buf, INFO_OFFSET_MAXVALUE) = info->maxvalue;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_NEXT) = info->next;

	memcpy(OFFSET(buf, INFO_OFFSET_DISTRIBUTION), info->distribution,
	    INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	return true;
}
