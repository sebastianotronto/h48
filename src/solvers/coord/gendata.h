STATIC size_t gendata_coord(const coord_t [static 1], unsigned char *);
STATIC int64_t gendata_coord_dispatch(const char *, unsigned char *);
STATIC tableinfo_t genptable_coord(
    const coord_t [static 1], const unsigned char *, unsigned char *);
STATIC bool switch_to_fromnew(uint64_t, uint64_t, uint64_t);
STATIC uint64_t genptable_coord_fillneighbors(const coord_t [static 1],
    const unsigned char *, uint64_t, uint8_t, unsigned char *);
STATIC uint64_t genptable_coord_fillfromnew(const coord_t [static 1],
    const unsigned char *, uint64_t, uint8_t, unsigned char *);
STATIC void getdistribution_coord(const unsigned char *, const char *,
    uint64_t [static INFO_DISTRIBUTION_LEN]);
STATIC uint8_t get_coord_pval(
    const coord_t [static 1], const unsigned char *, uint64_t);
STATIC void set_coord_pval(
    const coord_t [static 1], unsigned char *, uint64_t, uint8_t);

STATIC int64_t
gendata_coord_dispatch(const char *coordstr, unsigned char *buf)
{
	coord_t *coord;

	parse_coord_and_axis(strlen(coordstr), coordstr, &coord, NULL);

	if (coord == NULL) {
		LOG("Error: could not parse coordinate '%s'\n", coordstr);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	return (int64_t)gendata_coord(coord, buf);
}

STATIC size_t
gendata_coord(const coord_t coord[static 1], unsigned char *buf)
{
	uint64_t coord_dsize, tablesize, ninfo;
	unsigned char *pruningbuf, *coord_data;
	unsigned char *table;
	tableinfo_t coord_data_info, pruning_info;

	coord_data = buf == NULL ? NULL : buf + INFOSIZE;
	coord_dsize = coord->gendata(coord_data);
	if (coord_dsize == SIZE_MAX)
		goto gendata_coord_error;

	ninfo = coord_dsize == 0 ? 1 : 2;
	tablesize = DIV_ROUND_UP(coord->max, 2);

	if (buf == NULL)
		goto gendata_coord_return_size;

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

		pruningbuf = buf + INFOSIZE + coord_dsize;
	} else {
		pruningbuf = buf;
	}

	table = pruningbuf + INFOSIZE;
	pruning_info = genptable_coord(coord, coord_data, table);
	writetableinfo(&pruning_info, INFOSIZE + tablesize, pruningbuf);

gendata_coord_return_size:
	return ninfo * INFOSIZE + coord_dsize + tablesize;

gendata_coord_error:
	LOG("Unexpected error generating coordinate data\n");
	return 0;
}

STATIC tableinfo_t
genptable_coord(
	const coord_t coord[static 1],
	const unsigned char *data,
	unsigned char *table
)
{
	uint64_t tablesize, i, d, tot, t, nm;
	tableinfo_t info;

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

	nm = popcount_u32(coord->moves_mask);
	i = coord->coord(SOLVED_CUBE, data);
	set_coord_pval(coord, table, i, 0);
	info.distribution[0] = 1;
	for (d = 1, tot = 1; tot < coord->max && d < 20; d++) {
		t = 0;
		if (switch_to_fromnew(tot, coord->max, nm)) {
			for (i = 0; i < coord->max; i++)
				if (get_coord_pval(coord, table, i) > d)
					t += genptable_coord_fillfromnew(
					    coord, data, i, d, table);
		} else {
			for (i = 0; i < coord->max; i++)
				if (get_coord_pval(coord, table, i) == d-1)
					t += genptable_coord_fillneighbors(
					    coord, data, i, d, table);
		}
		tot += t;
		info.distribution[d] = t;
		LOG("[%s gendata] Depth %" PRIu64 ": found %" PRIu64 " (%"
		    PRIu64 " of %" PRIu64 ")\n",
		    coord->name, d, t, tot, coord->max);
	}
	info.maxvalue = d-1;

	return info;
}

STATIC bool
switch_to_fromnew(uint64_t done, uint64_t max, uint64_t nm)
{
	/*
	Heuristic to determine if it is more conveniente to loop over
      	done coordinates and fill the new discovered, or to loop from new
	coordinates and fill them if they have a done neighbor.
	*/

	double r = (double)done / (double)max;
	return 1.0 - intpow(1.0-r, nm) > nm * (1.0-r);
}

STATIC uint64_t
genptable_coord_fillneighbors(
	const coord_t coord[static 1],
	const unsigned char *data,
	uint64_t i, 
	uint8_t d,
	unsigned char *table
)
{
	bool isnasty;
	uint8_t m;
	uint64_t ii, j, t, tot;
	cube_t c, moved;

	c = coord->cube(i, data);
	tot = 0;
	for (m = 0; m < NMOVES; m++) {
		if (!((UINT32_C(1) << (uint32_t)m) & coord->moves_mask))
			continue;
		moved = move(c, m);
		ii = coord->coord(moved, data);
		isnasty = coord->isnasty(ii, data);
		for (t = 0; t < NTRANS && (t == 0 || isnasty); t++) {
			if (!((UINT64_C(1) << t) & coord->trans_mask))
				continue;

			j = coord->coord(transform(moved, t), data);
			if (get_coord_pval(coord, table, j) > d) {
				set_coord_pval(coord, table, j, d);
				tot++;
			}
		}
	}

	return tot;
}

STATIC uint64_t
genptable_coord_fillfromnew(
	const coord_t coord[static 1],
	const unsigned char *data,
	uint64_t i,
	uint8_t d,
	unsigned char *table
)
{
	bool found;
	uint8_t m;
	uint64_t tot, t, ii, j, nsim, sim[NTRANS];
	cube_t c;

	tot = 0;
	c = coord->cube(i, data);

	for (t = 0, nsim = 0; t < NTRANS; t++) {
		if (!((UINT64_C(1) << t) & coord->trans_mask))
			continue;

		ii = coord->coord(transform(c, t), data);
		for (j = 0, found = false; j < nsim && !found; j++)
			found = sim[j] == ii;
		if (!found)
			sim[nsim++] = ii;
	}

	for (j = 0, found = false; j < nsim && !found; j++) {
		c = coord->cube(sim[j], data);
		for (m = 0; m < NMOVES; m++) {
			if (!((UINT32_C(1) << (uint32_t)m) & coord->moves_mask))
				continue;
			ii = coord->coord(move(c, m), data);
			if (get_coord_pval(coord, table, ii) < d) {
				found = true;
				break;
			}
		}
	}

	tot = 0;
	if (found) {
		for (j = 0; j < nsim; j++) {
			if (get_coord_pval(coord, table, sim[j]) > d) {
				set_coord_pval(coord, table, sim[j], d);
				tot++;
			}
		}
	}

	return tot;
}

STATIC void
getdistribution_coord(
	const unsigned char *table,
	const char *coord,
	uint64_t distr[static INFO_DISTRIBUTION_LEN]
)
{
	uint8_t v;
	uint64_t i;
	coord_t *c;

	memset(distr, 0, INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	if((c = parse_coord(strlen(coord), coord)) == NULL)
		return;

	for (i = 0; i < c->max; i++) {
		v = get_coord_pval(c, table, i);
		distr[v]++;
	}
}

STATIC uint8_t
get_coord_pval(
	const coord_t coord[static 1],
	const unsigned char *table,
	uint64_t i
)
{
	return (table[COORD_INDEX(i)] & COORD_MASK(i)) >> COORD_SHIFT(i);
}

STATIC void
set_coord_pval(
	const coord_t coord[static 1],
	unsigned char *table,
	uint64_t i,
	uint8_t val
)
{
	table[COORD_INDEX(i)] = (table[COORD_INDEX(i)] & (~COORD_MASK(i)))
	    | (val << COORD_SHIFT(i));
}
