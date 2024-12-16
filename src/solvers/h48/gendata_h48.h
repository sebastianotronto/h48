STATIC uint64_t gendata_h48short(gendata_h48short_arg_t *);
STATIC int64_t gendata_h48(gendata_h48_arg_t *);
STATIC void gendata_h48h0k4(gendata_h48_arg_t *);
STATIC void gendata_h48k2(gendata_h48_arg_t *);
STATIC void gendata_h48k2_realcoord(gendata_h48_arg_t *);

STATIC void * gendata_h48h0k4_runthread(void *);
STATIC void * gendata_h48k2_runthread(void *);

STATIC_INLINE void gendata_h48_mark_atomic(gendata_h48_mark_t *);
STATIC_INLINE void gendata_h48_mark(gendata_h48_mark_t *);
STATIC_INLINE bool gendata_h48k2_dfs_stop(cube_t, int8_t, h48k2_dfs_arg_t *);
STATIC void gendata_h48k2_dfs(h48k2_dfs_arg_t *arg);
STATIC tableinfo_t makeinfo_h48k2(gendata_h48_arg_t *);
STATIC void getdistribution_h48(const uint8_t *,
    uint64_t [static INFO_DISTRIBUTION_LEN], uint8_t, uint8_t);

STATIC const uint32_t *get_cocsepdata_constptr(const void *);
STATIC const uint8_t *get_h48data_constptr(const void *);

STATIC_INLINE uint8_t get_h48_pval(const uint8_t *, int64_t, uint8_t);
STATIC_INLINE void set_h48_pval(uint8_t *, int64_t, uint8_t, uint8_t);
STATIC_INLINE uint8_t get_h48_pval_atomic(
    _Atomic const uint8_t *, int64_t, uint8_t);
STATIC_INLINE void set_h48_pval_atomic(
    _Atomic uint8_t *, int64_t, uint8_t, uint8_t);

size_t gendata_h48_derive(uint8_t, const void *, void *);

STATIC uint64_t
gendata_h48short(gendata_h48short_arg_t *arg)
{
	uint8_t i, m;
	int64_t coord;
	uint64_t j;
	kvpair_t kv;
	cube_t cube, d;

	cube = SOLVED_CUBE;
	coord = coord_h48(cube, arg->cocsepdata, 11);
	h48map_insertmin(arg->map, coord, 0);
	for (i = 0; i < arg->maxdepth; i++) {
		j = 0;
		for (kv = h48map_nextkvpair(arg->map, &j);
		     j != arg->map->capacity;
		     kv = h48map_nextkvpair(arg->map, &j)
		) {
			if (kv.val != i)
				continue;
			cube = invcoord_h48(kv.key, arg->crep, 11);
			for (m = 0; m < 18; m++) {
				d = move(cube, m);
				FOREACH_H48SIM(d, arg->cocsepdata, arg->selfsim,
					coord = coord_h48(d, arg->cocsepdata, 11);
					h48map_insertmin(arg->map, coord, i+1);
				)
			}
		}
	}

	return arg->map->n;
}

STATIC int64_t
gendata_h48(gendata_h48_arg_t *arg)
{
	uint64_t size, cocsepsize, h48size, fallbacksize, fallback2size, of;
	long long r;
	void *cocsepdata_offset;
	tableinfo_t cocsepinfo, h48info, fallbackinfo;
	gendata_h48_arg_t arg_h0k4;

	if (arg == NULL) {
		LOG("Error computing H48 data: arg is NULL.\n");
		return NISSY_ERROR_UNKNOWN;
	}

	cocsepsize = COCSEP_FULLSIZE;
	h48size = INFOSIZE + H48_TABLESIZE(arg->h, arg->k);
	fallbacksize = arg->k == 2 ? INFOSIZE + H48_TABLESIZE(0, 4) : 0;
	fallback2size = EOESEP_FULLSIZE;

	/* Add padding for 8-bit alignment */
	h48size = 8 * DIV_ROUND_UP(h48size, 8);
	fallbacksize = 8 * DIV_ROUND_UP(fallbacksize, 8);
	fallback2size = 8 * DIV_ROUND_UP(fallback2size, 8);

	size = cocsepsize + h48size + fallbacksize + fallback2size;

	if (arg->buf == NULL)
		return size; /* Dry-run */

	if (arg->buf_size < size) {
		LOG("Error computing H48 data: buffer is too small "
		    "(needed %" PRId64 " bytes but received %" PRId64 ")\n",
		    size, arg->buf_size);
		return NISSY_ERROR_BUFFER_SIZE;
	}

	gendata_cocsep(arg->buf, arg->selfsim, arg->crep);

	cocsepdata_offset = (char *)arg->buf + INFOSIZE;
	arg->cocsepdata = (uint32_t *)cocsepdata_offset;
	arg->h48buf = (char *)arg->buf + cocsepsize;

	arg->base = 99; /* TODO: set this somewhere else */

	if (arg->h == 0 && arg->k == 4) {
		gendata_h48h0k4(arg);
	} else if ((arg->h == 0 || arg->h == 11) && arg->k == 2) {
		gendata_h48k2_realcoord(arg);
	} else if (arg->k == 2) {
		gendata_h48k2(arg);
	} else {
		LOG("Cannot generate data for h = %" PRIu8 " and k = %" PRIu8
		    " (not implemented yet)\n", arg->h, arg->k);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	r = readtableinfo(arg->buf_size, arg->buf, &cocsepinfo);
	if (r != NISSY_OK) {
		LOG("gendata_h48: could not read info for cocsep table\n");
		return NISSY_ERROR_UNKNOWN;
	}

	cocsepinfo.next = cocsepsize;
	r = writetableinfo(&cocsepinfo, arg->buf_size, arg->buf);
	if (r != NISSY_OK) {
		LOG("gendata_h48: could not write info for cocsep table"
		    " with updated 'next' value\n");
		return NISSY_ERROR_UNKNOWN;
	}

	/* Add h0k4 fallback table */

	if (arg->k == 2) {
		arg_h0k4 = *arg;
		arg_h0k4.h = 0;
		arg_h0k4.k = 4;
		arg_h0k4.base = 0;
		arg_h0k4.maxdepth = 20;
		arg_h0k4.buf_size = arg->buf_size - h48size;
		arg_h0k4.buf = (char *)arg->buf + cocsepsize + h48size;
		arg_h0k4.h48buf = (char *)arg->h48buf + h48size;

		gendata_h48h0k4(&arg_h0k4);

	}

	/* Add eoesep fallback table */

	gendata_eoesep((char *)arg->buf + (size - fallback2size), 20);

	/* Update tableinfo with correct next values */

	r = readtableinfo_n(arg->buf_size, arg->buf, 2, &h48info);
	if (r != NISSY_OK) {
		LOG("gendata_h48: could not read info for h48 table\n");
		return NISSY_ERROR_UNKNOWN;
	}
	h48info.next = h48size;
	r = writetableinfo(&h48info,
	     arg->buf_size - cocsepsize, (char *)arg->buf + cocsepsize);
	if (r != NISSY_OK) {
		LOG("gendata_h48: could not write info for h48 table\n");
		return NISSY_ERROR_UNKNOWN;
	}

	if (arg->k == 2) {
		r = readtableinfo_n(arg->buf_size, arg->buf, 3, &fallbackinfo);
		if (r != NISSY_OK) {
			LOG("gendata_h48: could not read info for h48 "
			    "fallback table\n");
			return NISSY_ERROR_UNKNOWN;
		}

		of = cocsepsize + h48size;
		fallbackinfo.next = fallbacksize;
		r = writetableinfo(&fallbackinfo,
		    arg->buf_size - of, (char *)arg->buf + of);
		if (r != NISSY_OK) {
			LOG("gendata_h48: could not write info for h48 "
			    "fallback table\n");
			return NISSY_ERROR_UNKNOWN;
		}
	}

	return size;
}

STATIC void
gendata_h48h0k4(gendata_h48_arg_t *arg)
{
	_Atomic uint8_t *table;
	uint8_t val;
	int64_t i, sc, done, d, h48max;
	uint64_t t, tt, isize, cc, bufsize;
	h48h0k4_bfs_arg_t bfsarg[THREADS];
	pthread_t thread[THREADS];
	pthread_mutex_t table_mutex[CHUNKS];

	arg->info = (tableinfo_t) {
		.solver = "h48 solver h = 0, k = 4",
		.type = TABLETYPE_PRUNING,
		.infosize = INFOSIZE,
		.fullsize = H48_TABLESIZE(0, 4) + INFOSIZE,
		.hash = 0, /* TODO */
		.entries = H48_COORDMAX(0),
		.classes = 0,
		.h48h = 0,
		.bits = 4,
		.base = 0,
		.maxvalue = 0,
		.next = 0,
	};

	table = (_Atomic uint8_t *)arg->h48buf + INFOSIZE;
	memset(table, 0xFF, H48_TABLESIZE(0, 4));

	h48max = (int64_t)H48_COORDMAX(0);
	sc = coord_h48(SOLVED_CUBE, arg->cocsepdata, 0);
	set_h48_pval_atomic(table, sc, 4, 0);
	arg->info.distribution[0] = 1;

	isize = h48max / THREADS;
	isize = (isize / H48_COEFF(arg->k)) * H48_COEFF(arg->k);
	for (t = 0; t < CHUNKS; t++)
		pthread_mutex_init(&table_mutex[t], NULL);
	for (t = 0; t < THREADS; t++) {
		bfsarg[t] = (h48h0k4_bfs_arg_t) {
			.cocsepdata = arg->cocsepdata,
			.table = table,
			.selfsim = arg->selfsim,
			.crep = arg->crep,
			.start = isize * t,
			.end = t == THREADS-1 ? (uint64_t)h48max : isize * (t+1),
		};
		for (tt = 0; tt < CHUNKS; tt++)
			bfsarg[t].table_mutex[tt] = &table_mutex[tt];
	}
	for (done = 1, d = 1; done < h48max && d <= arg->maxdepth; d++) {
		LOG("h48: generating depth %" PRIu8 "\n", d);

		for (t = 0; t < THREADS; t++) {
			bfsarg[t].depth = d;
			pthread_create(&thread[t], NULL,
			    gendata_h48h0k4_runthread, &bfsarg[t]);
		}

		for (t = 0; t < THREADS; t++)
			pthread_join(thread[t], NULL);

		for (i = 0, cc = 0; i < h48max; i++) {
			val = get_h48_pval_atomic(table, i, 4);
			cc += val == d;
		}

		done += cc;
		arg->info.distribution[d] = cc;

		LOG("found %" PRId64 "\n", cc);
	}

	arg->info.maxvalue = d - 1;
	bufsize = arg->buf_size - COCSEP_FULLSIZE;
	writetableinfo(&arg->info, bufsize, arg->h48buf);
}

STATIC void *
gendata_h48h0k4_runthread(void *arg)
{
	static const uint8_t breakpoint = 10; /* Hand-picked optimal */

	uint8_t c, m;
	uint64_t i;
	int64_t j;
	cube_t cube, moved;
	gendata_h48_mark_t markarg;
	h48h0k4_bfs_arg_t *bfsarg;

	bfsarg = (h48h0k4_bfs_arg_t *)arg;

	markarg = (gendata_h48_mark_t) {
		.depth = bfsarg->depth,
		.h = 0,
		.k = 4,
		.cocsepdata = bfsarg->cocsepdata,
		.selfsim = bfsarg->selfsim,
		.table_atomic = bfsarg->table,
		.table_mutex = bfsarg->table_mutex,
	};

	/*
         * If depth < breakpoint, scan all neighbors of coordinates at depth-1.
         * Otherwise, scan all neighbors of unvisited coordinates.
	 */
	for (i = bfsarg->start; i < bfsarg->end; i++) {
		c = get_h48_pval_atomic(bfsarg->table, i, 4);

		if ((bfsarg->depth < breakpoint && c != bfsarg->depth - 1) ||
		    (bfsarg->depth >= breakpoint && c != 0xF))
			continue;

		cube = invcoord_h48(i, bfsarg->crep, 0);
		for (m = 0; m < 18; m++) {
			moved = move(cube, m);
			j = coord_h48(moved, bfsarg->cocsepdata, 0);
			c = get_h48_pval_atomic(bfsarg->table, j, 4);
			if (bfsarg->depth < breakpoint) {
				if (c <= bfsarg->depth)
					continue;
				markarg.cube = moved;
				gendata_h48_mark_atomic(&markarg);
			} else {
				if (c >= bfsarg->depth)
					continue;
				markarg.cube = cube;
				gendata_h48_mark_atomic(&markarg);
				break; /* Enough to find one, skip the rest */
			}
		}
	}

	return NULL;
}

STATIC void
gendata_h48k2(gendata_h48_arg_t *arg)
{
	static const uint8_t shortdepth = 8;
	static const uint64_t capacity = 10000019;
	static const uint64_t randomizer = 10000079;

	/*
	 * A good base value for the k=2 tables have few positions with value
	 * 0, because those are treated as lower bound 0 and require a second
	 * lookup in another table, and at the same time not too many positions
	 * with value 3, because some of those are under-estimates.
	 *
	 * The following values for the base have been hand-picked. I first
	 * performed some statistics on the frequency of these values, but
	 * they turned out to be unreliable. In the end I generated the same
	 * table with multiple base value and see what was best.
	 *
	 * A curious case is h3, which has this distribution for base 8:
	 *   [0] = 6686828
	 *   [1] = 63867852
	 *   [2] = 392789689
	 *   [3] = 477195231
	 *
	 * and this for base 9:
	 *   [0] = 70554680
	 *   [1] = 392789689
	 *   [2] = 462294676
	 *   [3] = 14900555
	 *
	 * I ended up picking base 8 to have a much lower count of elements
	 * with value 0, at the cost of a less precise estimate for the higher
	 * values. But I am not 100% confident this is the optimal choice,
	 * so I'll leave it here for future considerations.
	 */
	 
	static const uint8_t base[] = {
		[0]  = 8,
		[1]  = 8,
		[2]  = 8,
		[3]  = 8,
		[4]  = 9,
		[5]  = 9,
		[6]  = 9,
		[7]  = 9,
		[8]  = 10,
		[9]  = 10,
		[10] = 10,
		[11] = 10
	};

	uint8_t t;
	uint8_t *table;
	int64_t j;
	uint64_t i, ii, inext, count, bufsize;
	h48map_t shortcubes;
	gendata_h48short_arg_t shortarg;
	h48k2_dfs_arg_t dfsarg[THREADS];
	pthread_t thread[THREADS];
	pthread_mutex_t shortcubes_mutex, table_mutex[CHUNKS];

	table = (uint8_t *)arg->h48buf + INFOSIZE;
	memset(table, 0xFF, H48_TABLESIZE(arg->h, arg->k));

	LOG("Computing depth <=%" PRIu8 "\n", shortdepth)
	h48map_create(&shortcubes, capacity, randomizer);
	shortarg = (gendata_h48short_arg_t) {
		.maxdepth = shortdepth,
		.cocsepdata = arg->cocsepdata,
		.crep = arg->crep,
		.selfsim = arg->selfsim,
		.map = &shortcubes
	};
	gendata_h48short(&shortarg);
	LOG("Computed %" PRIu64 " positions\n", shortarg.map->n);

	if (arg->base >= 20)
		arg->base = base[arg->h];
	arg->info = makeinfo_h48k2(arg);

	inext = count = 0;
	pthread_mutex_init(&shortcubes_mutex, NULL);
	for (i = 0; i < CHUNKS; i++)
		pthread_mutex_init(&table_mutex[i], NULL);
	for (i = 0; i < THREADS; i++) {
		dfsarg[i] = (h48k2_dfs_arg_t){
			.h = arg->h,
			.k = arg->k,
			.base = arg->base,
			.shortdepth = shortdepth,
			.cocsepdata = arg->cocsepdata,
			.table = table,
			.selfsim = arg->selfsim,
			.crep = arg->crep,
			.shortcubes = &shortcubes,
			.shortcubes_mutex = &shortcubes_mutex,
			.next = &inext,
			.count = &count,
		};
		for (ii = 0; ii < CHUNKS; ii++)
			dfsarg[i].table_mutex[ii] = &table_mutex[ii];

		pthread_create(
		    &thread[i], NULL, gendata_h48k2_runthread, &dfsarg[i]);
	}

	for (i = 0; i < THREADS; i++)
		pthread_join(thread[i], NULL);

	h48map_destroy(&shortcubes);

	for (j = 0; j < H48_COORDMAX(arg->h); j++) {
		t = get_h48_pval(table, j, 2);
		arg->info.distribution[t]++;
	}

	bufsize = arg->buf_size - COCSEP_FULLSIZE;
	writetableinfo(&arg->info, bufsize, arg->h48buf);
}

STATIC void *
gendata_h48k2_runthread(void *arg)
{
	uint64_t count, coord, mutex;
	kvpair_t kv;
	h48k2_dfs_arg_t *dfsarg;

	dfsarg = (h48k2_dfs_arg_t *)arg;

	while (true) {
		pthread_mutex_lock(dfsarg->shortcubes_mutex);

		kv = h48map_nextkvpair(dfsarg->shortcubes, dfsarg->next);
		if (*dfsarg->next == dfsarg->shortcubes->capacity) {
			pthread_mutex_unlock(dfsarg->shortcubes_mutex);
			break;
		}
		count = ++(*dfsarg->count);
		pthread_mutex_unlock(dfsarg->shortcubes_mutex);

		if (count % UINT64_C(1000000) == 0)
			LOG("Processing %" PRIu64 "th short cube\n", count);

		if (kv.val < dfsarg->shortdepth) {
			coord = kv.key >> (int64_t)(11 - dfsarg->h);
			mutex = H48_INDEX(coord, dfsarg->k) % CHUNKS;
			pthread_mutex_lock(dfsarg->table_mutex[mutex]);
			set_h48_pval(dfsarg->table, coord, dfsarg->k, 0);
			pthread_mutex_unlock(dfsarg->table_mutex[mutex]);
		} else {
			dfsarg->cube = invcoord_h48(kv.key, dfsarg->crep, 11);
			gendata_h48k2_dfs(dfsarg);
		}
	}

	return NULL;
}

STATIC void
gendata_h48k2_dfs(h48k2_dfs_arg_t *arg)
{
	int8_t d;
	uint8_t m[4];
	cube_t cube[4];
	gendata_h48_mark_t markarg;

	markarg = (gendata_h48_mark_t) {
		.h = arg->h,
		.k = arg->k,
		.cocsepdata = arg->cocsepdata,
		.selfsim = arg->selfsim,
		.table = arg->table,
		.table_mutex = arg->table_mutex,
	};

	d = (int8_t)arg->shortdepth - (int8_t)arg->base;

	/* Depth d+0 (shortcubes) */
	markarg.depth = d;
	markarg.cube = arg->cube;
	gendata_h48_mark(&markarg);

	/* Depth d+1 */
	for (m[0] = 0; m[0] < 18; m[0]++) {
		markarg.depth = d+1;
		cube[0] = move(arg->cube, m[0]);
		if (gendata_h48k2_dfs_stop(cube[0], d+1, arg))
			continue;
		markarg.cube = cube[0];
		gendata_h48_mark(&markarg);

		/* Depth d+2 */
		for (m[1] = 0; m[1] < 18; m[1]++) {
			markarg.depth = d+2;
			if (m[0] / 3 == m[1] / 3) {
				m[1] += 2;
				continue;
			}
			cube[1] = move(cube[0], m[1]);
			if (gendata_h48k2_dfs_stop(cube[1], d+2, arg))
				continue;
			markarg.cube = cube[1];
			gendata_h48_mark(&markarg);
			if (d >= 0)
				continue;

			/* Depth d+3 */
			for (m[2] = 0; m[2] < 18; m[2]++) {
				markarg.depth = d+3;
				if (!allowednextmove(m, 3)) {
					m[2] += 2;
					continue;
				}
				cube[2] = move(cube[1], m[2]);
				if (gendata_h48k2_dfs_stop(cube[2], d+3, arg))
					continue;
				markarg.cube = cube[2];
				gendata_h48_mark(&markarg);
				if (d >= -1)
					continue;

				/* Depth d+4 */
				for (m[3] = 0; m[3] < 18; m[3]++) {
					markarg.depth = d+4;
					if (!allowednextmove(m, 4)) {
						m[3] += 2;
						continue;
					}
					cube[3] = move(cube[2], m[3]);
					markarg.cube = cube[3];
					gendata_h48_mark(&markarg);
				}
			}
		}
	}
}

STATIC_INLINE void
gendata_h48_mark_atomic(gendata_h48_mark_t *arg)
{
	uint8_t oldval, newval;
	int64_t coord, mutex;

	FOREACH_H48SIM(arg->cube, arg->cocsepdata, arg->selfsim,
		coord = coord_h48(arg->cube, arg->cocsepdata, arg->h);
		oldval = get_h48_pval_atomic(arg->table_atomic, coord, arg->k);
		newval = (uint8_t)MAX(arg->depth, 0);
		if (newval < oldval) {
			mutex = H48_INDEX(coord, arg->k) % CHUNKS;
			pthread_mutex_lock(arg->table_mutex[mutex]);
			set_h48_pval_atomic(
			    arg->table_atomic, coord, arg->k, newval);
			pthread_mutex_unlock(arg->table_mutex[mutex]);
		}
	)
}

STATIC_INLINE void
gendata_h48_mark(gendata_h48_mark_t *arg)
{
	uint8_t oldval, newval;
	int64_t coord, mutex;

	FOREACH_H48SIM(arg->cube, arg->cocsepdata, arg->selfsim,
		coord = coord_h48(arg->cube, arg->cocsepdata, arg->h);
		mutex = H48_INDEX(coord, arg->k) % CHUNKS;
		pthread_mutex_lock(arg->table_mutex[mutex]);
		oldval = get_h48_pval(arg->table, coord, arg->k);
		newval = (uint8_t)MAX(arg->depth, 0);
		set_h48_pval(arg->table, coord, arg->k, MIN(newval, oldval));
		pthread_mutex_unlock(arg->table_mutex[mutex]);
	)
}

STATIC_INLINE bool
gendata_h48k2_dfs_stop(cube_t cube, int8_t depth, h48k2_dfs_arg_t *arg)
{
	uint64_t val;
	int64_t coord, mutex;
	int8_t oldval;

	if (arg->h == 0 || arg->h == 11) {
		/* We are in the "real coordinate" case, we can stop
		   if this coordinate has already been visited */
		coord = coord_h48(cube, arg->cocsepdata, arg->h);
		mutex = H48_INDEX(coord, arg->k) % CHUNKS;
		pthread_mutex_lock(arg->table_mutex[mutex]);
		oldval = get_h48_pval(arg->table, coord, arg->k);
		pthread_mutex_unlock(arg->table_mutex[mutex]);
		return oldval <= depth;
	} else {
		/* With 0 < k < 11 we do not have a "real coordinate".
		   The best we can do is checking if we backtracked to
		   one of the "short cubes". */
		coord = coord_h48(cube, arg->cocsepdata, 11);
		val = h48map_value(arg->shortcubes, coord);
		return val <= arg->shortdepth;
	}
}

STATIC void
gendata_h48k2_realcoord(gendata_h48_arg_t *arg)
{
	/* TODO */
	gendata_h48k2(arg);
}

STATIC void *
gendata_h48k2_realcoord_runthread(void *arg)
{
	/* TODO */
	return NULL;
}

STATIC tableinfo_t
makeinfo_h48k2(gendata_h48_arg_t *arg)
{
	tableinfo_t info;

	info = (tableinfo_t) {
		.solver = "h48 solver h =  , k = 2",
		.type = TABLETYPE_PRUNING,
		.infosize = INFOSIZE,
		.fullsize = H48_TABLESIZE(arg->h, 2) + INFOSIZE,
		.hash = 0, /* TODO */
		.entries = H48_COORDMAX(arg->h),
		.classes = 0,
		.h48h = arg->h,
		.bits = 2,
		.base = arg->base,
		.maxvalue = 3,
		.next = 0,
	};
	info.solver[15] = (arg->h % 10) + '0';
	if (arg->h >= 10)
		info.solver[14] = (arg->h / 10) + '0';

	return info;
}

STATIC void
getdistribution_h48(
	const uint8_t *table,
	uint64_t distr[static INFO_DISTRIBUTION_LEN],
	uint8_t h,
	uint8_t k
) {
	uint8_t val;
	int64_t i, h48max;

	memset(distr, 0, INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	h48max = H48_COORDMAX(h);
	for (i = 0; i < h48max; i++) {
		val = get_h48_pval(table, i, k);
		distr[val]++;
	}
}
STATIC const uint32_t *
get_cocsepdata_constptr(const void *data)
{
	return (uint32_t *)((char *)data + INFOSIZE);
}

STATIC const uint8_t *
get_h48data_constptr(const void *data)
{
	return (uint8_t *)data + COCSEP_FULLSIZE + INFOSIZE;
}

STATIC_INLINE uint8_t
get_h48_pval(const uint8_t *table, int64_t i, uint8_t k)
{
	return (table[H48_INDEX(i, k)] & H48_MASK(i, k)) >> H48_SHIFT(i, k);
}

STATIC_INLINE uint8_t
get_h48_pval_atomic(_Atomic const uint8_t *table, int64_t i, uint8_t k)
{
	return (table[H48_INDEX(i, k)] & H48_MASK(i, k)) >> H48_SHIFT(i, k);
}

STATIC_INLINE void
set_h48_pval(uint8_t *table, int64_t i, uint8_t k, uint8_t val)
{
	table[H48_INDEX(i, k)] = (table[H48_INDEX(i, k)] & (~H48_MASK(i, k)))
	    | (val << H48_SHIFT(i, k));
}

STATIC_INLINE void
set_h48_pval_atomic(_Atomic uint8_t *table, int64_t i, uint8_t k, uint8_t val)
{
	table[H48_INDEX(i, k)] = (table[H48_INDEX(i, k)] & (~H48_MASK(i, k)))
	    | (val << H48_SHIFT(i, k));
}

size_t
gendata_h48_derive(uint8_t h, const void *fulltable, void *buf)
{
	size_t cocsepsize, h48size;
	uint8_t val_full, val_derive;
	const uint8_t *h48full;
	uint8_t *h48derive;
	int64_t i, j, h48max;
	uint64_t bufsize;
	gendata_h48_arg_t arg;
	tableinfo_t cocsepinfo, fulltableinfo;

	/* Initializing values in case of error */
	/* TODO cleanup this */
	fulltableinfo.h48h = 11;
	fulltableinfo.bits = 2;
	fulltableinfo.base = 8;

	int64_t TODOlarge = 999999999999; /* TODO: cleanup here */

	readtableinfo_n(TODOlarge, fulltable, 2, &fulltableinfo);
	arg.h = h;
	arg.k = fulltableinfo.bits;
	arg.maxdepth = 20;
	arg.buf = buf;
	arg.cocsepdata = (uint32_t *)((char *)buf + INFOSIZE);
	arg.base = fulltableinfo.base;
	arg.info = makeinfo_h48k2(&arg);

	/* Technically this step is redundant, except that we
	   need selfsim and crep */
	cocsepsize = gendata_cocsep(buf, arg.selfsim, arg.crep);
	arg.h48buf = (_Atomic uint8_t *)buf + cocsepsize;
	h48size = H48_TABLESIZE(h, arg.k) + INFOSIZE;

	if (buf == NULL)
		goto gendata_h48_derive_return_size;

	bufsize = COCSEP_FULLSIZE + INFOSIZE;
	if (readtableinfo(bufsize, buf, &cocsepinfo) != NISSY_OK) {
		LOG("gendata_h48: could not read info for cocsep table\n");
		goto gendata_h48_derive_error;
	}

	cocsepinfo.next = cocsepsize;
	bufsize = COCSEP_FULLSIZE + INFOSIZE;
	if (writetableinfo(&cocsepinfo, bufsize, buf) != NISSY_OK) {
		LOG("gendata_h48_derive: could not write info for cocsep table"
		    " with updated 'next' value\n");
		goto gendata_h48_derive_error;
	}

	h48full = (const uint8_t *)fulltable + cocsepsize + INFOSIZE;
	h48derive = (uint8_t *)arg.h48buf + INFOSIZE;
	memset(h48derive, 0xFF, H48_TABLESIZE(h, arg.k));
	memset(arg.info.distribution, 0,
	    INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	h48max = H48_COORDMAX(fulltableinfo.h48h);
	for (i = 0; i < h48max; i++) {
		if (i % INT64_C(1000000000) == 0 && i > 0)
			LOG("Processing %" PRId64 "th coordinate\n", i);
		j = i >> (int64_t)(fulltableinfo.h48h - h);
		val_full = get_h48_pval(h48full, i, arg.k);
		val_derive = get_h48_pval(h48derive, j, arg.k);
		set_h48_pval(
		    h48derive, j, arg.k, MIN(val_full, val_derive));
	}

	getdistribution_h48(h48derive, arg.info.distribution, h, arg.k);

	bufsize = arg.buf_size - COCSEP_FULLSIZE - INFOSIZE;
	if (writetableinfo(&arg.info, bufsize, arg.h48buf) != NISSY_OK) {
		LOG("gendata_h48_derive: could not write info for table\n");
		goto gendata_h48_derive_error;
	}

gendata_h48_derive_return_size:
	return cocsepsize + h48size;

gendata_h48_derive_error:
	return 0;
}
