/* Type definitions and macros are in a separate file for easier testing */
#include "tables_types_macros.h"

STATIC uint64_t read_unaligned_u64(const void *);
STATIC void write_unaligned_u64(void *, uint64_t);
STATIC bool readtableinfo(const void *, tableinfo_t *);
STATIC bool readtableinfo_n(const void *, uint8_t, tableinfo_t *);
STATIC bool writetableinfo(const tableinfo_t *, void *);

STATIC uint64_t
read_unaligned_u64(const void *buf)
{
	uint64_t ret;

	memcpy(&ret, buf, sizeof(uint64_t));

	return ret;
}

STATIC void
write_unaligned_u64(void *buf, uint64_t x)
{
	memcpy(buf, &x, sizeof(uint64_t));
}

STATIC bool
readtableinfo(const void *buf, tableinfo_t *info)
{
	size_t i;

	if (buf == NULL) {
		LOG("Error reading table: buffer is NULL\n");
		return false;
	}

	if (info == NULL) {
		LOG("Error reading table info: info struct is NULL\n");
		return false;
	}

	for (i = 0; i < INFO_DISTRIBUTION_LEN; i++)
		info->distribution[i] = read_unaligned_u64(OFFSET(buf,
		    INFO_OFFSET_DISTRIBUTION + i * sizeof(uint64_t)));

	info->type = read_unaligned_u64(OFFSET(buf, INFO_OFFSET_TYPE));
	info->infosize = read_unaligned_u64(OFFSET(buf, INFO_OFFSET_INFOSIZE));
	info->fullsize = read_unaligned_u64(OFFSET(buf, INFO_OFFSET_FULLSIZE));
	info->hash = read_unaligned_u64(OFFSET(buf, INFO_OFFSET_HASH));
	info->entries = read_unaligned_u64(OFFSET(buf, INFO_OFFSET_ENTRIES));
	info->classes = read_unaligned_u64(OFFSET(buf, INFO_OFFSET_CLASSES));
	info->next = read_unaligned_u64(OFFSET(buf, INFO_OFFSET_NEXT));

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
	size_t i;
	bool end;
	uint8_t *c;

	if (buf == NULL) {
		LOG("Error writing table: buffer is NULL\n");
		return false;
	}

	if (info == NULL) {
		LOG("Error writing table info: provided info is NULL\n");
		return false;
	}

	for (i = 0; i < INFO_DISTRIBUTION_LEN; i++)
		write_unaligned_u64(OFFSET(buf, INFO_OFFSET_DISTRIBUTION +
		    i * sizeof(uint64_t)), info->distribution[i]);

	write_unaligned_u64(OFFSET(buf, INFO_OFFSET_TYPE), info->type);
	write_unaligned_u64(OFFSET(buf, INFO_OFFSET_INFOSIZE), info->infosize);
	write_unaligned_u64(OFFSET(buf, INFO_OFFSET_FULLSIZE), info->fullsize);
	write_unaligned_u64(OFFSET(buf, INFO_OFFSET_HASH), info->hash);
	write_unaligned_u64(OFFSET(buf, INFO_OFFSET_ENTRIES), info->entries);
	write_unaligned_u64(OFFSET(buf, INFO_OFFSET_CLASSES), info->classes);
	write_unaligned_u64(OFFSET(buf, INFO_OFFSET_NEXT), info->next);

	memcpy(OFFSET(buf, INFO_OFFSET_SOLVER), info->solver,
	    INFO_SOLVER_STRLEN);

	/* Zeroing all chars after the end of the string, for consistency */
	end = false;
	for (i = 0; i < INFO_SOLVER_STRLEN; i++) {
		c = OFFSET(buf, INFO_OFFSET_SOLVER + i);
		end = end || *c == 0;
		if (end)
			*c = 0;
	}

	*OFFSET(buf, INFO_OFFSET_H48H) = info->h48h;
	*OFFSET(buf, INFO_OFFSET_BITS) = info->bits;
	*OFFSET(buf, INFO_OFFSET_BASE) = info->base;
	*OFFSET(buf, INFO_OFFSET_MAXVALUE) = info->maxvalue;

	return true;
}
