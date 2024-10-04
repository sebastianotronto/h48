/* Type definitions and macros are in a separate file for easier testing */
#include "tables_types_macros.h"

STATIC bool readtableinfo(const void *, tableinfo_t *);
STATIC bool readtableinfo_n(const void *, uint8_t, tableinfo_t *);
STATIC bool writetableinfo(const tableinfo_t *, void *);

STATIC bool
readtableinfo(const void *buf, tableinfo_t *info)
{
	if (buf == NULL) {
		LOG("Error reading table: buffer is NULL\n");
		return false;
	}

	if (info == NULL) {
		LOG("Error reading table info: info struct is NULL\n");
		return false;
	}

	memcpy(info->distribution, OFFSET(buf, INFO_OFFSET_DISTRIBUTION),
	    INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	info->type = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_TYPE);
	info->infosize = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_INFOSIZE);
	info->fullsize = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_FULLSIZE);
	info->hash = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_HASH);
	info->entries = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_ENTRIES);
	info->classes = *(const uint64_t *)OFFSET(buf, INFO_OFFSET_CLASSES);
	info->next = *(const uint64_t* )OFFSET(buf, INFO_OFFSET_NEXT);

	memcpy(info->solver, OFFSET(buf, INFO_OFFSET_SOLVER),
	    INFO_SOLVER_STRLEN);

	info->h48h = *OFFSET(buf, INFO_OFFSET_H48H);
	info->bits = *OFFSET(buf, INFO_OFFSET_BITS);
	info->base = *OFFSET(buf, INFO_OFFSET_BASE);
	info->maxvalue = *OFFSET(buf, INFO_OFFSET_MAXVALUE);

	return true;
}

STATIC bool
readtableinfo_n(const void *buf, uint8_t n, tableinfo_t *info)
{
	for ( ; n > 0; n--, buf = (char *)buf + info->next)
		if (!readtableinfo(buf, info))
			return false;

	return true;
}

STATIC bool
writetableinfo(const tableinfo_t *info, void *buf)
{
	int i;

	if (buf == NULL) {
		LOG("Error writing table: buffer is NULL\n");
		return false;
	}

	if (info == NULL) {
		LOG("Error writing table info: provided info is NULL\n");
		return false;
	}

	memcpy(OFFSET(buf, INFO_OFFSET_DISTRIBUTION), info->distribution,
	    INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	*(uint64_t *)OFFSET(buf, INFO_OFFSET_TYPE) = info->type;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_INFOSIZE) = info->infosize;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_FULLSIZE) = info->fullsize;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_HASH) = info->hash;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_ENTRIES) = info->entries;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_CLASSES) = info->classes;
	*(uint64_t *)OFFSET(buf, INFO_OFFSET_NEXT) = info->next;

	memcpy(OFFSET(buf, INFO_OFFSET_SOLVER), info->solver,
	    INFO_SOLVER_STRLEN);

	/* Zeroing all chars after the end of the string, for consistency */
	for (i = 1; i < INFO_SOLVER_STRLEN; i++)
		if (*OFFSET(buf, i) == 0)
			*OFFSET(buf, i) = 0;

	*OFFSET(buf, INFO_OFFSET_H48H) = info->h48h;
	*OFFSET(buf, INFO_OFFSET_BITS) = info->bits;
	*OFFSET(buf, INFO_OFFSET_BASE) = info->base;
	*OFFSET(buf, INFO_OFFSET_MAXVALUE) = info->maxvalue;

	return true;
}
