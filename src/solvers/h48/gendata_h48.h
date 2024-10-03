#define H48_COORDMAX_NOEO    ((int64_t)(COCSEP_CLASSES * COMB_12_4 * COMB_8_4))
#define H48_COORDMAX(h)      (H48_COORDMAX_NOEO << (int64_t)(h))
#define H48_DIV(k)           ((size_t)8 / (size_t)(k))
#define H48_TABLESIZE(h, k)  DIV_ROUND_UP((size_t)H48_COORDMAX((h)), H48_DIV(k))

#define H48_COEFF(k)         (INT64_C(8) / (int64_t)(k))
#define H48_INDEX(i, k)      ((i) / H48_COEFF(k))
#define H48_SHIFT(i, k)      ((uint8_t)(k) * (uint8_t)((i) % H48_COEFF(k)))
#define H48_MASK(i, k)       ((UINT8_BIT(k) - UINT8_C(1)) << H48_SHIFT(i, k))

#define MAXLEN 20
#define CHUNKS COCSEP_CLASSES

/*
TODO: This loop over similar h48 coordinates can be improved by only
transforming edges, but we need to compose transformations (i.e. conjugate
_t by _ttrep).
*/
#define FOREACH_H48SIM(ARG_CUBE, ARG_COCSEPDATA, ARG_SELFSIM, ARG_ACTION) \
	int64_t VAR_COCSEP = coord_cocsep(ARG_CUBE); \
	uint8_t VAR_TTREP = TTREP(ARG_COCSEPDATA[VAR_COCSEP]); \
	uint8_t VAR_INVERSE_TTREP = inverse_trans(VAR_TTREP); \
	int64_t VAR_COCLASS = COCLASS(ARG_COCSEPDATA[VAR_COCSEP]); \
	cube_t VAR_REP = transform(ARG_CUBE, VAR_TTREP); \
	uint64_t VAR_S = ARG_SELFSIM[VAR_COCLASS]; \
	for (uint8_t VAR_T = 0; VAR_T < 48 && VAR_S; VAR_T++, VAR_S >>= 1) { \
		if (!(VAR_S & 1)) continue; \
		ARG_CUBE = transform(VAR_REP, VAR_T); \
		ARG_CUBE = transform(ARG_CUBE, VAR_INVERSE_TTREP); \
		ARG_ACTION \
	}

typedef struct {
	uint8_t h;
	uint8_t k;
	uint8_t base;
	uint8_t maxdepth;
	tableinfo_t info;
	void *buf;
	void *h48buf;
	uint32_t *cocsepdata;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t crep[COCSEP_CLASSES];
} gendata_h48_arg_t;

typedef struct {
	uint8_t maxdepth;
	const uint32_t *cocsepdata;
	const cube_t *crep;
	const uint64_t *selfsim;
	h48map_t *map;
} gendata_h48short_arg_t;

typedef struct {
	uint8_t depth;
	uint32_t *cocsepdata;
	_Atomic uint8_t *table;
	uint64_t *selfsim;
	cube_t *crep;
	uint64_t start;
	uint64_t end;
	uint64_t count;
	uint64_t *distribution;
	pthread_mutex_t *distribution_mutex;
	pthread_mutex_t *table_mutex[CHUNKS];
} h48h0k4_bfs_arg_t;

typedef struct {
	cube_t cube;
	uint8_t h;
	uint8_t k;
	uint8_t base;
	uint8_t shortdepth;
	uint32_t *cocsepdata;
	_Atomic uint8_t *table;
	uint64_t *selfsim;
	cube_t *crep;
	h48map_t *shortcubes;
	pthread_mutex_t *shortcubes_mutex;
	pthread_mutex_t *table_mutex[CHUNKS];
	uint64_t *next;
	uint64_t *count;
} h48k2_dfs_arg_t;

typedef struct {
	cube_t cube;
	int8_t depth;
	uint8_t h;
	uint8_t k;
	uint32_t *cocsepdata;
	uint64_t *selfsim;
	_Atomic uint8_t *table;
	pthread_mutex_t **table_mutex;
} gendata_h48_mark_t;

STATIC uint64_t gendata_h48short(gendata_h48short_arg_t *);
STATIC size_t gendata_h48(gendata_h48_arg_t *);
STATIC size_t gendata_h48h0k4(gendata_h48_arg_t *);
STATIC size_t gendata_h48k2(gendata_h48_arg_t *);
STATIC void * gendata_h48h0k4_runthread(void *);
STATIC_INLINE uint64_t gendata_h48_mark(gendata_h48_mark_t *);
STATIC_INLINE bool gendata_h48k2_dfs_stop(cube_t, int8_t, h48k2_dfs_arg_t *);
STATIC size_t gendata_h48k2_realcoord(gendata_h48_arg_t *);
STATIC void gendata_h48k2_dfs(h48k2_dfs_arg_t *arg);
STATIC void * gendata_h48k2_runthread(void *);
STATIC tableinfo_t makeinfo_h48k2(gendata_h48_arg_t *);
STATIC void getdistribution_h48(const uint8_t *,
    uint64_t [static INFO_DISTRIBUTION_LEN], uint8_t, uint8_t);

STATIC const uint32_t *get_cocsepdata_constptr(const void *);
STATIC const uint8_t *get_h48data_constptr(const void *);

STATIC_INLINE uint8_t get_h48_pval(const uint8_t *, int64_t, uint8_t);
STATIC_INLINE void set_h48_pval(_Atomic uint8_t *, int64_t, uint8_t, uint8_t);
STATIC_INLINE uint8_t get_h48_bound(
    cube_t, uint32_t, uint8_t, uint8_t, const uint8_t *);

size_t gendata_h48_derive(uint8_t, const void *, void *);

STATIC uint64_t
gendata_h48short(gendata_h48short_arg_t *arg)
{
	uint8_t i, m;
	int64_t coord;
	uint64_t j, oldn;
	kvpair_t kv;
	cube_t cube, d;

	cube = SOLVED_CUBE;
	coord = coord_h48(cube, arg->cocsepdata, 11);
	h48map_insertmin(arg->map, coord, 0);
	oldn = 0;
	LOG("Short h48: depth 0\nfound %" PRIu8 "\n", arg->map->n-oldn);
	for (i = 0; i < arg->maxdepth; i++) {
		LOG("Short h48: depth %" PRIu8 "\n", i+1);
		j = 0;
		oldn = arg->map->n;
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
		LOG("found %" PRIu8 "\n", arg->map->n-oldn);
	}

	return arg->map->n;
}

/* Generic function that dispatches to the data generators */
STATIC size_t
gendata_h48(gendata_h48_arg_t *arg)
{
	void *cocsepdata_offset;
	size_t cocsepsize, h48size;
	tableinfo_t cocsepinfo;

	cocsepsize = gendata_cocsep(arg->buf, arg->selfsim, arg->crep);

	cocsepdata_offset = (char *)arg->buf + INFOSIZE;
	arg->cocsepdata = (uint32_t *)cocsepdata_offset;
	arg->h48buf = (char *)arg->buf + cocsepsize;

	arg->base = 99; // TODO: set this somewhere else

	if (arg->h == 0 && arg->k == 4) {
		h48size = gendata_h48h0k4(arg);
	} else if ((arg->h == 0 || arg->h == 11) && arg->k == 2) {
		h48size = gendata_h48k2_realcoord(arg);
	} else if (arg->k == 2) {
		h48size = gendata_h48k2(arg);
	} else {
		LOG("Cannot generate data for h = %" PRIu8 " and k = %" PRIu8
		    " (not implemented yet)\n", arg->h, arg->k);
		goto gendata_h48_error;
	}

	if (arg->buf == NULL)
		goto gendata_h48_return_size;

	if (!readtableinfo(arg->buf, &cocsepinfo)) {
		LOG("gendata_h48: could not read info for cocsep table\n");
		goto gendata_h48_error;
	}

	cocsepinfo.next = cocsepsize;
	if (!writetableinfo(&cocsepinfo, arg->buf)) {
		LOG("gendata_h48: could not write info for cocsep table"
		    " with updated 'next' value\n");
		goto gendata_h48_error;
	}

gendata_h48_return_size:
	return cocsepsize + h48size;

gendata_h48_error:
	return 0;
}

STATIC size_t
gendata_h48h0k4(gendata_h48_arg_t *arg)
{
	_Atomic uint8_t *table;
	int64_t sc, done, d, h48max;
	uint64_t t, tt, isize, cc;
	h48h0k4_bfs_arg_t bfsarg[THREADS];
	pthread_t thread[THREADS];
	pthread_mutex_t distribution_mutex, table_mutex[CHUNKS];

	if (arg->buf == NULL)
		goto gendata_h48h0k4_return_size;

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
	set_h48_pval(table, sc, 4, 0);
	arg->info.distribution[0] = 1;

	isize = h48max / THREADS;
	isize = (isize / H48_COEFF(arg->k)) * H48_COEFF(arg->k);
	pthread_mutex_init(&distribution_mutex, NULL);
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
			.distribution = arg->info.distribution,
			.distribution_mutex = &distribution_mutex,
		};
		for (tt = 0; tt < CHUNKS; tt++)
			bfsarg[t].table_mutex[tt] = &table_mutex[tt];
	}
	for (done = 1, d = 1; done < h48max && d <= arg->maxdepth; d++) {
		LOG("h48: generating depth %" PRIu8 "\n", d);

		for (t = 0; t < THREADS; t++) {
			bfsarg[t].depth = d;
			bfsarg[t].count = 0;
			pthread_create(&thread[t], NULL,
			    gendata_h48h0k4_runthread, &bfsarg[t]);
		}

		for (t = 0, cc = 0; t < THREADS; t++) {
			pthread_join(thread[t], NULL);
			cc += bfsarg[t].count;
		}

		done += cc;
		arg->info.distribution[d] = cc;

		LOG("found %" PRId64 "\n", cc);
	}

	arg->info.maxvalue = d - 1;
	writetableinfo(&arg->info, arg->h48buf);

gendata_h48h0k4_return_size:
	return H48_TABLESIZE(0, 4) + INFOSIZE;
}

STATIC void *
gendata_h48h0k4_runthread(void *arg)
{
	static const uint8_t breakpoint = 10; /* Hand-picked optimal */

	uint8_t c, m;
	uint64_t i, d;
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
		.table = bfsarg->table,
		.table_mutex = bfsarg->table_mutex,
	};

	/*
         * If depth < breakpoint, scan all neighbors of coordinates at depth-1.
         * Otherwise, scan all neighbors of unvisited coordinates.
	 */
	for (i = bfsarg->start, d = 0; i < bfsarg->end; i++) {
		c = get_h48_pval((uint8_t *)bfsarg->table, i, 4);

		if ((bfsarg->depth < breakpoint && c != bfsarg->depth - 1) ||
		    (bfsarg->depth >= breakpoint && c != 0xF))
			continue;

		cube = invcoord_h48(i, bfsarg->crep, 0);
		for (m = 0; m < 18; m++) {
			moved = move(cube, m);
			j = coord_h48(moved, bfsarg->cocsepdata, 0);
			c = get_h48_pval((uint8_t *)bfsarg->table, j, 4);
			if (bfsarg->depth < breakpoint) {
				if (c <= bfsarg->depth)
					continue;
				markarg.cube = moved;
				d += gendata_h48_mark(&markarg);
			} else {
				if (c >= bfsarg->depth)
					continue;
				markarg.cube = cube;
				d += gendata_h48_mark(&markarg);
				break; /* Enough to find one, skip the rest */
			}
		}
	}

	pthread_mutex_lock(bfsarg->distribution_mutex);
	bfsarg->count += d;
	bfsarg->distribution[bfsarg->depth] += d;
	pthread_mutex_unlock(bfsarg->distribution_mutex);

	return NULL;
}

STATIC size_t
gendata_h48k2(gendata_h48_arg_t *arg)
{
	static const uint8_t shortdepth = 8;
	static const uint64_t capacity = 10000019;
	static const uint64_t randomizer = 10000079;
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
	_Atomic uint8_t *table;
	int64_t j;
	uint64_t i, ii, inext, count;
	h48map_t shortcubes;
	gendata_h48short_arg_t shortarg;
	h48k2_dfs_arg_t dfsarg[THREADS];
	pthread_t thread[THREADS];
	pthread_mutex_t shortcubes_mutex, table_mutex[CHUNKS];

	if (arg->buf == NULL)
		goto gendata_h48k2_return_size;

	table = (_Atomic uint8_t *)arg->h48buf + INFOSIZE;
	if (arg->buf != NULL)
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

	/* TODO: inline into mark */
	for (j = 0; j < H48_COORDMAX(arg->h); j++) {
		t = get_h48_pval((uint8_t *)table, j, 2);
		arg->info.distribution[t]++;
	}

	writetableinfo(&arg->info, arg->h48buf);

gendata_h48k2_return_size:
	return H48_TABLESIZE(arg->h, 2) + INFOSIZE;
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

STATIC_INLINE uint64_t
gendata_h48_mark(gendata_h48_mark_t *arg)
{
	uint8_t oldval, newval;
	uint64_t d;
	int64_t coord, mutex;

	d = 0;
	FOREACH_H48SIM(arg->cube, arg->cocsepdata, arg->selfsim,
		coord = coord_h48(arg->cube, arg->cocsepdata, arg->h);
		mutex = H48_INDEX(coord, arg->k) % CHUNKS;
		pthread_mutex_lock(arg->table_mutex[mutex]);
		oldval = get_h48_pval((uint8_t *)arg->table, coord, arg->k);
		newval = (uint8_t)MAX(arg->depth, 0);
		d += newval < oldval;
		set_h48_pval(arg->table, coord, arg->k, MIN(oldval, newval));
		pthread_mutex_unlock(arg->table_mutex[mutex]);
	)

	return d;
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
		oldval = get_h48_pval((uint8_t *)arg->table, coord, arg->k);
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

STATIC size_t
gendata_h48k2_realcoord(gendata_h48_arg_t *arg)
{
	/* TODO */
	return gendata_h48k2(arg);
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

STATIC_INLINE void
set_h48_pval(_Atomic uint8_t *table, int64_t i, uint8_t k, uint8_t val)
{
	table[H48_INDEX(i, k)] = (table[H48_INDEX(i, k)] & (~H48_MASK(i, k)))
	    | (val << H48_SHIFT(i, k));
}

STATIC_INLINE uint8_t
get_h48_bound(
	cube_t cube,
	uint32_t cdata,
	uint8_t h,
	uint8_t k,
	const uint8_t *table
) {
	int64_t coord;

	coord = coord_h48_edges(cube, COCLASS(cdata), TTREP(cdata), h);
	return get_h48_pval(table, coord, k);
}

size_t
gendata_h48_derive(uint8_t h, const void *fulltable, void *buf)
{
	size_t cocsepsize, h48size;
	uint8_t val_full, val_derive;
	const uint8_t *h48full;
	_Atomic uint8_t *h48derive;
	int64_t i, j, h48max;
	gendata_h48_arg_t arg;
	tableinfo_t cocsepinfo, fulltableinfo;

	/* Initializing values in case of error */
	/* TODO cleanup this */
	fulltableinfo.bits = 2;
	fulltableinfo.base = 8;

	readtableinfo_n(fulltable, 2, &fulltableinfo);
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

	if (!readtableinfo(buf, &cocsepinfo)) {
		LOG("gendata_h48: could not read info for cocsep table\n");
		goto gendata_h48_derive_error;
	}

	cocsepinfo.next = cocsepsize;
	if (!writetableinfo(&cocsepinfo, buf)) {
		LOG("gendata_h48_derive: could not write info for cocsep table"
		    " with updated 'next' value\n");
		goto gendata_h48_derive_error;
	}

	h48full = (const uint8_t *)fulltable + cocsepsize + INFOSIZE;
	h48derive = (_Atomic uint8_t *)arg.h48buf + INFOSIZE;
	memset(h48derive, 0xFF, H48_TABLESIZE(h, arg.k));
	memset(arg.info.distribution, 0,
	    INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	h48max = H48_COORDMAX(fulltableinfo.h48h);
	for (i = 0; i < h48max; i++) {
		if (i % INT64_C(1000000000) == 0 && i > 0)
			LOG("Processing %" PRId64 "th coordinate\n", i);
		j = i >> (int64_t)(fulltableinfo.h48h - h);
		val_full = get_h48_pval(h48full, i, arg.k);
		val_derive = get_h48_pval((uint8_t *)h48derive, j, arg.k);
		set_h48_pval(
		    h48derive, j, arg.k, MIN(val_full, val_derive));
	}

	getdistribution_h48(
	    (uint8_t *)h48derive, arg.info.distribution, h, arg.k);

	if (!writetableinfo(&arg.info, arg.h48buf)) {
		LOG("gendata_h48_derive: could not write info for table\n");
		goto gendata_h48_derive_error;
	}

gendata_h48_derive_return_size:
	return cocsepsize + h48size;

gendata_h48_derive_error:
	return 0;
}
