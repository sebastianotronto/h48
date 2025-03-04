STATIC size_t gendata_coordinate(const coord_t *, void *);
STATIC size_t gendata_coordinate_name(const char *, void *);
STATIC tableinfo_t genptable_coordinate(const coord_t *, const void *, uint8_t *);
STATIC uint8_t get_coord_pval(const coord_t *, const uint8_t *, uint64_t);
STATIC void set_coord_pval(const coord_t *, uint8_t *, uint64_t, uint8_t);

STATIC size_t
gendata_coordinate_name(const char *name, void *buf)
{
	int i;

	for (i = 0; all_coordinates[i] != NULL; i++)
		if (strcmp(all_coordinates[i]->name, name) == 0)
			return gendata_coordinate(all_coordinates[i], buf);

	LOG("Cannot generate data for coordinate '%s': not found\n", name);
	return 0;
}

STATIC size_t
gendata_coordinate(const coord_t *coord, void *buf)
{
	uint64_t coord_dsize, tablesize, ninfo;
	void *pruningbuf, *coord_data;
	uint8_t *table;
	tableinfo_t coord_data_info, pruning_info;

	coord_data = buf == NULL ? NULL : ((uint8_t *)buf) + INFOSIZE;
	coord_dsize = coord->gendata(coord_data);
	ninfo = coord_dsize == 0 ? 1 : 2;
	tablesize = DIV_ROUND_UP(coord->max, 2);

	if (buf == NULL)
		goto gendata_coordinate_return_size;

	if (ninfo == 2) {
		coord_data_info = (tableinfo_t) {
			.solver = "coord helper table for ",
			.type = TABLETYPE_SPECIAL,
			.infosize = INFOSIZE,
			.fullsize = INFOSIZE + coord_dsize,
			.hash = 0, /* TODO */
			.next = INFOSIZE + coord_dsize,

			/* Unknown / non-applicable values */
			.entries = 0,
			.classes = 0,
			.bits = 0,
			.base = 0,
			.maxvalue = 0,
		};

		append_coord_name(coord, coord_data_info.solver);

		writetableinfo(&coord_data_info, INFOSIZE + coord_dsize, buf);

		pruningbuf = ((uint8_t *)buf) + INFOSIZE + coord_dsize;
	} else {
		pruningbuf = buf;
	}

	table = ((uint8_t *)pruningbuf) + INFOSIZE;
	pruning_info = genptable_coordinate(coord, coord_data, table);
	writetableinfo(&pruning_info, INFOSIZE + tablesize, pruningbuf);

gendata_coordinate_return_size:
	return ninfo * INFOSIZE + coord_dsize + tablesize;
}

STATIC tableinfo_t
genptable_coordinate(const coord_t *coord, const void *data, uint8_t *table)
{
	uint64_t tablesize, i, j, d, tot;
	tableinfo_t info;
	uint8_t m;
	cube_t c, cc;

	tablesize = DIV_ROUND_UP(coord->max, 2);

	memset(table, 0xFF, tablesize);

	info = (tableinfo_t) {
		.solver = "coordinate solver for ",
		.type = TABLETYPE_PRUNING,
		.infosize = INFOSIZE,
		.fullsize = INFOSIZE + tablesize,
		.hash = 0, /* TODO */
		.entries = coord->max,
		.classes = 0,
		.bits = 4,
		.base = 0,
		.maxvalue = 0,
		.next = 0
	};

	memset(info.distribution, 0, INFO_DISTRIBUTION_LEN * sizeof(uint64_t));
	append_coord_name(coord, info.solver);

	i = coord->coord(SOLVED_CUBE, data);
	set_coord_pval(coord, table, i, 0);
	info.distribution[0] = 1;
	for (d = 1, tot = 1; tot < coord->max; d++) {
		for (i = 0; i < coord->max; i++) {
			if (get_coord_pval(coord, table, i) == d-1) {
				c = coord->cube(i, data);
				for (m = 0; m < 18; m++) {
					cc = move(c, m);
					j = coord->coord(cc, data);
					if (get_coord_pval(coord, table, j) > d) {
						set_coord_pval(coord, table, j, d);
						tot++;
						info.distribution[d]++;
					}
				}
			}
		}
	}
	info.maxvalue = d-1;

	return info;
}

STATIC uint8_t
get_coord_pval(const coord_t *coord, const uint8_t *table, uint64_t i)
{
	return (table[COORD_INDEX(i)] & COORD_MASK(i)) >> COORD_SHIFT(i);
}

STATIC void
set_coord_pval(const coord_t *coord, uint8_t *table, uint64_t i, uint8_t val)
{
	table[COORD_INDEX(i)] = (table[COORD_INDEX(i)] & (~COORD_MASK(i)))
	    | (val << COORD_SHIFT(i));
}
