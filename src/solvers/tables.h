STATIC uint64_t read_unaligned_u64(const char [static sizeof(uint64_t)]);
STATIC void write_unaligned_u64(char [static sizeof(uint64_t)], uint64_t);
STATIC int64_t readtableinfo(size_t n, const char [n], tableinfo_t [static 1]);
STATIC int64_t readtableinfo_n(
    size_t n, const char [n], uint8_t, tableinfo_t [static 1]);
STATIC int64_t writetableinfo(
    const tableinfo_t [static 1], size_t n, char [n]);

STATIC uint64_t
read_unaligned_u64(const char buf[static sizeof(uint64_t)])
{
	uint64_t ret;

	memcpy(&ret, buf, sizeof(uint64_t));

	return ret;
}

STATIC void
write_unaligned_u64(char buf[static sizeof(uint64_t)], uint64_t x)
{
	memcpy(buf, &x, sizeof(uint64_t));
}

STATIC int64_t
readtableinfo(
	size_t buf_size,
	const char buf[buf_size],
	tableinfo_t info[static 1]
)
{
	size_t i;

	if (buf == NULL) {
		LOG("Error reading table: buffer is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	if (buf_size < INFOSIZE) {
		LOG("Error reading table: buffer size is too small "
		    "(given size %" PRIu64 " is smaller than INFOSIZE = %"
		    PRId64 ")\n", buf_size, INFOSIZE);
		return NISSY_ERROR_BUFFER_SIZE;
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

	info->h48h = *(uint8_t *)OFFSET(buf, INFO_OFFSET_H48H);
	info->bits = *(uint8_t *)OFFSET(buf, INFO_OFFSET_BITS);
	info->base = *(uint8_t *)OFFSET(buf, INFO_OFFSET_BASE);
	info->maxvalue = *(uint8_t *)OFFSET(buf, INFO_OFFSET_MAXVALUE);

	return NISSY_OK;
}

STATIC int64_t
readtableinfo_n(
	size_t buf_size,
	const char buf[buf_size],
	uint8_t n,
	tableinfo_t info[static 1]
)
{
	int64_t ret;

	for (; n > 0; n--, buf = buf + info->next, buf_size -= info->next)
		if ((ret = readtableinfo(buf_size, buf, info)) != 0)
			return ret;

	return NISSY_OK;
}

STATIC int64_t
writetableinfo(
	const tableinfo_t info[static 1],
	size_t data_size,
	char buf[data_size]
)
{
	size_t i;
	bool end;
	char *c;

	if (data_size < info->fullsize) {
		LOG("Error writing table: buffer size is too small "
		    "(given %" PRId64 " but table requires %" PRId64 ")\n",
		    data_size, info->fullsize);
		return NISSY_ERROR_BUFFER_SIZE;
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

	*(uint8_t *)OFFSET(buf, INFO_OFFSET_H48H) = info->h48h;
	*(uint8_t *)OFFSET(buf, INFO_OFFSET_BITS) = info->bits;
	*(uint8_t *)OFFSET(buf, INFO_OFFSET_BASE) = info->base;
	*(uint8_t *)OFFSET(buf, INFO_OFFSET_MAXVALUE) = info->maxvalue;

	return NISSY_OK;
}
