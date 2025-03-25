STATIC int64_t coord_eoesep_sym(cube_t, const uint32_t [static ESEP_MAX]);
STATIC size_t gendata_esep_classes(
    uint32_t [static ESEP_MAX], uint16_t [static ESEP_CLASSES]);
STATIC size_t gendata_eoesep(char *, uint8_t);
STATIC uint32_t gendata_eoesep_bfs(uint8_t, uint8_t [static EOESEP_BUF],
    uint32_t [static ESEP_MAX], uint16_t [static ESEP_CLASSES]);
STATIC uint32_t gendata_eoesep_fromnew(uint8_t, uint8_t [static EOESEP_BUF],
    uint32_t [static ESEP_MAX], uint16_t [static ESEP_CLASSES]);
STATIC uint32_t gendata_eoesep_fromdone(uint8_t, uint8_t [static EOESEP_BUF],
    uint32_t [static ESEP_MAX], uint16_t [static ESEP_CLASSES]);
STATIC uint32_t gendata_eoesep_marksim(int64_t, uint8_t,
    uint8_t [static EOESEP_BUF], uint32_t [static ESEP_MAX]);
STATIC bool gendata_eoesep_next(cube_t, uint8_t,
    uint8_t [static EOESEP_BUF], uint32_t [static ESEP_MAX]);
STATIC uint8_t get_eoesep_pval(
    const uint8_t [static DIV_ROUND_UP(EOESEP_TABLESIZE, 2)], int64_t);
STATIC uint8_t get_eoesep_pval_cube(const void *, cube_t);
STATIC void set_eoesep_pval(
    uint8_t [static DIV_ROUND_UP(EOESEP_TABLESIZE, 2)], int64_t, uint8_t);

STATIC int64_t
coord_eoesep_sym(cube_t c, const uint32_t esep_classes[static ESEP_MAX])
{
	uint8_t ttrep;
	uint32_t edata, class;
	int64_t esep, eo;

	esep = coord_esep(c);
	edata = esep_classes[esep];
	class = ECLASS(edata);
	ttrep = TTREP(edata);
	eo = coord_eo(transform(c, ttrep));

	return (class << UINT32_C(11)) + eo;
}

STATIC size_t
gendata_esep_classes(
	uint32_t esep_classes[static ESEP_MAX],
	uint16_t rep[static ESEP_CLASSES]
)
{
	bool visited[ESEP_MAX];
	uint8_t t;
	uint32_t class, cl, ti;
	int64_t i, j;
	cube_t c;

	memset(visited, 0, ESEP_MAX * sizeof(bool));
	class = 0;
	for (i = 0; i < ESEP_MAX; i++) {
		if (visited[i])
			continue;
		c = invcoord_esep(i);
		for (t = 0; t < NTRANS; t++) {
			j = coord_esep(transform(c, t));
			cl = class << UINT32_C(16);
			ti = inverse_trans(t) << UINT32_C(8);
			esep_classes[j] = cl | ti;
			visited[j] = true;
		}
		rep[class] = i;
		class++;
	}

	return class;
}

STATIC size_t
gendata_eoesep(char *buf, uint8_t maxdepth)
{
	uint8_t *buf8, d;
	uint16_t rep[ESEP_CLASSES];
	uint32_t *esep_classes, done, level;
	int64_t coord;
	tableinfo_t info;

	if (buf == NULL)
		goto gendata_eoesep_return_size;

	LOG("Computing eoesep data\n");
	memset(buf, 0xFF, EOESEP_FULLSIZE);
	esep_classes = (uint32_t *)(buf + INFOSIZE);
	buf8 = (uint8_t *)(buf + INFOSIZE + 4*ESEP_MAX);
	gendata_esep_classes(esep_classes, rep);

	info = (tableinfo_t) {
		.solver = "eoesep data for h48",
		.type = TABLETYPE_SPECIAL,
		.infosize = INFOSIZE,
		.fullsize = EOESEP_FULLSIZE,
		.hash = 0,
		.entries = EOESEP_TABLESIZE,
		.classes = ESEP_CLASSES,
		.bits = 4,
		.base = 0,
		.maxvalue = 11,
		.next = 0
	};

	coord = 0; /* Assumed coordinate of solved cube */
	set_eoesep_pval(buf8, coord, 0);
	done = 1;
	info.distribution[0] = 1;
	for (d = 1; d <= maxdepth && done < EOESEP_TABLESIZE; d++) {
		level = gendata_eoesep_bfs(d, buf8, esep_classes, rep);
		done += level;
		info.distribution[d] = level;
	}

	writetableinfo(&info, EOESEP_FULLSIZE, buf);

	LOG("eoesep data computed\n");

gendata_eoesep_return_size:
	return EOESEP_FULLSIZE;
}

STATIC uint32_t
gendata_eoesep_bfs(
	uint8_t d,
	uint8_t buf8[EOESEP_BUF],
	uint32_t esep_classes[static ESEP_MAX],
	uint16_t rep[static ESEP_CLASSES]
)
{
	if (d < 9)
		return gendata_eoesep_fromdone(d, buf8, esep_classes, rep);
	else
		return gendata_eoesep_fromnew(d, buf8, esep_classes, rep);
}

STATIC uint32_t
gendata_eoesep_fromdone(
	uint8_t d,
	uint8_t buf8[EOESEP_BUF],
	uint32_t esep_classes[static ESEP_MAX],
	uint16_t rep[static ESEP_CLASSES]
)
{
	uint8_t pval;
	int64_t i, esep, eo, coord, done;

	done = 0;
	for (i = 0; i < (int64_t)ESEP_CLASSES; i++) {
		esep = rep[i];
		for (eo = 0; eo < POW_2_11; eo++) {
			coord = (i << INT64_C(11)) + eo;
			pval = get_eoesep_pval(buf8, coord);
			if (pval != d-1)
				continue;

			coord = (esep << INT64_C(11)) + eo;
			done += gendata_eoesep_marksim(
			    coord, d, buf8, esep_classes);
		}
	}

	return done;
}

STATIC uint32_t
gendata_eoesep_fromnew(
	uint8_t d,
	uint8_t buf8[EOESEP_BUF],
	uint32_t esep_classes[static ESEP_MAX],
	uint16_t rep[static ESEP_CLASSES]
)
{
	uint8_t pval;
	int64_t i, esep, eo, coord, done;
	cube_t c;

	done = 0;
	for (i = 0; i < (int64_t)ESEP_CLASSES; i++) {
		esep = rep[i];
		for (eo = 0; eo < POW_2_11; eo++) {
			coord = (i << INT64_C(11)) + eo;
			pval = get_eoesep_pval(buf8, coord);
			if (pval != 15)
				continue;

			c = invcoord_eoesep((esep << INT64_C(11)) + eo);
			if (gendata_eoesep_next(c, d, buf8, esep_classes)) {
				set_eoesep_pval(buf8, coord, d);
				done++;
			}
		}
	}

	return done;
}

STATIC uint32_t
gendata_eoesep_marksim(
	int64_t i,
	uint8_t d,
	uint8_t buf8[static EOESEP_BUF],
	uint32_t esep_classes[static ESEP_MAX]
)
{
	uint8_t t, m, pval;
	cube_t c, moved, transformed;
	uint32_t done;
	int64_t coord;

	done = 0;
	c = invcoord_eoesep(i);
	for (m = 0; m < 18; m++) {
		moved = move(c, m);
		for (t = 0; t < NTRANS; t++) {
			transformed = transform(moved, t);
			coord = coord_eoesep_sym(transformed, esep_classes);
			pval = get_eoesep_pval(buf8, coord);
			if (pval > d) {
				set_eoesep_pval(buf8, coord, d);
				done++;
			}
		}
	}

	return done;
}

STATIC bool
gendata_eoesep_next(
	cube_t c,
	uint8_t d,
	uint8_t buf8[static EOESEP_BUF],
	uint32_t esep_classes[static ESEP_MAX]
)
{
	uint8_t m, t, pval;
	int64_t coord;
	cube_t moved, transformed;

	for (t = 0; t < NTRANS; t++) {
		transformed = transform(c, t);
		for (m = 0; m < 18; m++) {
			moved = move(transformed, m);
			coord = coord_eoesep_sym(moved, esep_classes);
			pval = get_eoesep_pval(buf8, coord);
			if (pval == d-1)
				return true;
		}
	}

	return false;
}

STATIC uint8_t
get_eoesep_pval(
	const uint8_t table[static DIV_ROUND_UP(EOESEP_TABLESIZE, 2)],
	int64_t i
)
{
	return (table[EOESEP_INDEX(i)] & EOESEP_MASK(i)) >> EOESEP_SHIFT(i);
}

STATIC uint8_t
get_eoesep_pval_cube(const void *data, cube_t c)
{
	int64_t coord;
	const uint8_t *table;
	const uint32_t *esep_classes;

	esep_classes = (const uint32_t *)data;
	table = (const uint8_t *)data + 4*ESEP_MAX;
	coord = coord_eoesep_sym(c, esep_classes);

	return get_eoesep_pval(table, coord);
}

STATIC void
set_eoesep_pval(
	uint8_t table[static DIV_ROUND_UP(EOESEP_TABLESIZE, 2)],
	int64_t i,
	uint8_t val
)
{
	table[EOESEP_INDEX(i)] = (table[EOESEP_INDEX(i)] & (~EOESEP_MASK(i)))
	    | (val << EOESEP_SHIFT(i));
}
