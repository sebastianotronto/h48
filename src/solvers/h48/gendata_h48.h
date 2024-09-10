#define H48_COORDMAX_NOEO    ((int64_t)(COCSEP_CLASSES * COMB_12_4 * COMB_8_4))
#define H48_COORDMAX(h)      (H48_COORDMAX_NOEO << (int64_t)(h))
#define H48_DIV(k)           ((size_t)8 / (size_t)(k))
#define H48_TABLESIZE(h, k)  DIV_ROUND_UP((size_t)H48_COORDMAX((h)), H48_DIV(k))

#define H48_COEFF(k)         (INT64_C(8) / (int64_t)(k))
#define H48_INDEX(i, k)      ((i) / H48_COEFF(k))
#define H48_SHIFT(i, k)      ((uint8_t)(k) * (uint8_t)((i) % H48_COEFF(k)))
#define H48_MASK(i, k)       ((UINT8_BIT(k) - UINT8_C(1)) << H48_SHIFT(i, k))

#define MAXLEN 20

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
	uint8_t *table;
	uint64_t *selfsim;
	int64_t done;
	cube_t *crep;
} h48h0k4_bfs_arg_t;

typedef struct {
	cube_t cube;
	uint8_t h;
	uint8_t k;
	uint8_t base;
	uint8_t shortdepth;
	uint32_t *cocsepdata;
	uint8_t *table;
	uint64_t *selfsim;
	cube_t *crep;
	h48map_t *shortcubes;
} h48k2_dfs_arg_t;

STATIC_INLINE uint8_t get_h48_pval(const uint8_t *, int64_t, uint8_t);
STATIC_INLINE void set_h48_pval(uint8_t *, int64_t, uint8_t, uint8_t);
STATIC_INLINE uint8_t get_h48_bound(cube_t, uint32_t, uint8_t, uint8_t, uint8_t *);

STATIC uint64_t gen_h48short(gendata_h48short_arg_t *);
STATIC size_t gendata_h48(gendata_h48_arg_t *);
STATIC size_t gendata_h48h0k4(gendata_h48_arg_t *);
STATIC int64_t gendata_h48h0k4_bfs(h48h0k4_bfs_arg_t *);
STATIC int64_t gendata_h48h0k4_bfs_fromdone(h48h0k4_bfs_arg_t *);
STATIC int64_t gendata_h48h0k4_bfs_fromnew(h48h0k4_bfs_arg_t *);
STATIC size_t gendata_h48k2(gendata_h48_arg_t *);
STATIC_INLINE void gendata_h48k2_mark(cube_t, int8_t, h48k2_dfs_arg_t *);
STATIC_INLINE bool gendata_h48k2_dfs_stop(cube_t, uint8_t, h48k2_dfs_arg_t *);
STATIC void gendata_h48k2_dfs(h48k2_dfs_arg_t *arg);

STATIC uint64_t
gen_h48short(gendata_h48short_arg_t *arg)
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

	if (arg->h == 0 && arg->k == 4) {
		h48size = gendata_h48h0k4(arg);
	} else if (arg->k == 2) {
		h48size = gendata_h48k2(arg);
	} else {
		LOG("Cannot generate data for h = %" PRIu8 " and k = %" PRIu8
		    " (not implemented yet)\n", arg->h, arg->k);
		goto gendata_h48_error;
	}

	if (arg->buf == 0)
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

/*
TODO description
generating fixed table with h=0, k=4
*/
STATIC size_t
gendata_h48h0k4(gendata_h48_arg_t *arg)
{
	uint32_t j;
	uint8_t *table;
	h48h0k4_bfs_arg_t bfsarg;
	int64_t sc, cc, h48max;

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
		.bits = 4,
		.base = 0,
		.next = 0,
	};

	table = (uint8_t *)arg->h48buf + INFOSIZE;
	memset(table, 0xFF, H48_TABLESIZE(0, 4));

	h48max = (int64_t)H48_COORDMAX(0);
	sc = coord_h48(SOLVED_CUBE, arg->cocsepdata, 0);
	set_h48_pval(table, sc, 4, 0);
	arg->info.distribution[0] = 1;
	bfsarg = (h48h0k4_bfs_arg_t) {
		.cocsepdata = arg->cocsepdata,
		.table = table,
		.selfsim = arg->selfsim,
		.crep = arg->crep
	};
	for (
		bfsarg.done = 1, bfsarg.depth = 1, cc = 0;
		bfsarg.done < h48max && bfsarg.depth <= arg->maxdepth;
		bfsarg.depth++
	) {
		LOG("h48: generating depth %" PRIu8 "\n", bfsarg.depth);
		cc = gendata_h48h0k4_bfs(&bfsarg);
		bfsarg.done += cc;
		arg->info.distribution[bfsarg.depth] = cc;
		LOG("found %" PRId64 "\n", cc);
	}

	arg->info.maxvalue = bfsarg.depth-1;

	LOG("h48 pruning table computed\n");
	LOG("Maximum pruning value: %" PRIu32 "\n", arg->info.maxvalue);
	LOG("Pruning value distribution:\n");
	for (j = 0; j <= arg->info.maxvalue; j++)
		LOG("%" PRIu8 ":\t%" PRIu32 "\n", j, arg->info.distribution[j]);

	writetableinfo(&arg->info, arg->h48buf);

gendata_h48h0k4_return_size:
	return H48_TABLESIZE(0, 4) + INFOSIZE;
}

STATIC int64_t
gendata_h48h0k4_bfs(h48h0k4_bfs_arg_t *arg)
{
	const uint8_t breakpoint = 10; /* Hand-picked optimal */

	if (arg->depth < breakpoint)
		return gendata_h48h0k4_bfs_fromdone(arg);
	else
		return gendata_h48h0k4_bfs_fromnew(arg);
}

STATIC int64_t
gendata_h48h0k4_bfs_fromdone(h48h0k4_bfs_arg_t *arg)
{
	uint8_t c, m, x;
	uint32_t cc;
	int64_t i, j, k;
	cube_t cube, moved;

	for (i = 0, cc = 0; i < (int64_t)H48_COORDMAX(0); i++) {
		c = get_h48_pval(arg->table, i, 4);
		if (c != arg->depth - 1)
			continue;
		cube = invcoord_h48(i, arg->crep, 0);
		for (m = 0; m < 18; m++) {
			moved = move(cube, m);
			j = coord_h48(moved, arg->cocsepdata, 0);
			if (get_h48_pval(arg->table, j, 4) <= arg->depth)
				continue;
			FOREACH_H48SIM(moved, arg->cocsepdata, arg->selfsim,
				k = coord_h48(moved, arg->cocsepdata, 0);
				x = get_h48_pval(arg->table, k, 4);
				set_h48_pval(arg->table, k, 4, arg->depth);
				cc += x != arg->depth;
			)
		}
	}

	return cc;
}

STATIC int64_t
gendata_h48h0k4_bfs_fromnew(h48h0k4_bfs_arg_t *arg)
{
	uint8_t c, m, x;
	uint32_t cc;
	int64_t i, j;
	cube_t cube, moved;

	for (i = 0, cc = 0; i < (int64_t)H48_COORDMAX(0); i++) {
		c = get_h48_pval(arg->table, i, 4);
		if (c != 0xF)
			continue;
		cube = invcoord_h48(i, arg->crep, 0);
		for (m = 0; m < 18; m++) {
			moved = move(cube, m);
			j = coord_h48(moved, arg->cocsepdata, 0);
			x = get_h48_pval(arg->table, j, 4);
			if (x >= arg->depth)
				continue;
			FOREACH_H48SIM(cube, arg->cocsepdata, arg->selfsim,
				j = coord_h48(cube, arg->cocsepdata, 0);
				x = get_h48_pval(arg->table, j, 4);
				set_h48_pval(arg->table, j, 4, arg->depth);
				cc += x == 0xF;
			)
			break; /* Enough to find one, skip the rest */
		}
	}

	return cc;
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

	uint8_t t, selectedbase, *table;
	int64_t j;
	uint64_t nshort, i, ii;
	h48map_t shortcubes;
	kvpair_t kv;
	gendata_h48short_arg_t shortarg;
	h48k2_dfs_arg_t dfsarg;

	if (arg->buf == NULL)
		goto gendata_h48k2_return_size;

	table = (uint8_t *)arg->h48buf + INFOSIZE;
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
	nshort = gen_h48short(&shortarg);
	LOG("Cubes in <= %" PRIu8 " moves: %" PRIu64 "\n", shortdepth, nshort);

	selectedbase = base[arg->h];
	arg->info = (tableinfo_t) {
		.solver = "h48 solver h =  , k = 4",
		.type = TABLETYPE_PRUNING,
		.infosize = INFOSIZE,
		.fullsize = H48_TABLESIZE(arg->h, 2) + INFOSIZE,
		.hash = 0, /* TODO */
		.entries = H48_COORDMAX(arg->h),
		.bits = 2,
		.base = selectedbase,
		.next = 0,
	};
	arg->info.solver[15] = (arg->h % 10) + '0';
	if (arg->h >= 10)
		arg->info.solver[14] = (arg->h / 10) + '0';

	dfsarg = (h48k2_dfs_arg_t){
		.h = arg->h,
		.k = arg->k,
		.base = selectedbase,
		.shortdepth = shortdepth,
		.cocsepdata = arg->cocsepdata,
		.table = table,
		.selfsim = arg->selfsim,
		.crep = arg->crep,
		.shortcubes = &shortcubes
	};

	i = ii = 0;
	for (kv = h48map_nextkvpair(&shortcubes, &i);
	     i != shortcubes.capacity;
	     kv = h48map_nextkvpair(&shortcubes, &i)
	) {
		dfsarg.cube = invcoord_h48(kv.key, arg->crep, 11);
		gendata_h48k2_dfs(&dfsarg);
		if (++ii % UINT64_C(1000000) == 0)
			LOG("Processed %" PRIu64 " short cubes\n", ii);
	}

	h48map_destroy(&shortcubes);

	arg->info.base = selectedbase;
	for (j = 0; j < H48_COORDMAX(arg->h); j++) {
		t = get_h48_pval(table, j, 2);
		arg->info.distribution[t]++;
	}

	writetableinfo(&arg->info, arg->h48buf);

gendata_h48k2_return_size:
	return H48_TABLESIZE(arg->h, 2) + INFOSIZE;
}

STATIC void
gendata_h48k2_dfs(h48k2_dfs_arg_t *arg)
{
	int8_t d;
	uint8_t m[4];
	cube_t cube[4];

	d = (int8_t)arg->shortdepth - (int8_t)arg->base;

	/* Depth d+0 (shortcubes) */
	gendata_h48k2_mark(arg->cube, d, arg);

	/* Depth d+1 */
	for (m[0] = 0; m[0] < 18; m[0]++) {
		cube[0] = move(arg->cube, m[0]);
		if (gendata_h48k2_dfs_stop(cube[0], d+1, arg))
			continue;
		gendata_h48k2_mark(cube[0], d+1, arg);

		/* Depth d+2 */
		for (m[1] = 0; m[1] < 18; m[1]++) {
			if (m[0] / 3 == m[1] / 3) {
				m[1] += 2;
				continue;
			}
			cube[1] = move(cube[0], m[1]);
			if (gendata_h48k2_dfs_stop(cube[1], d+2, arg))
				continue;
			gendata_h48k2_mark(cube[1], d+2, arg);
			if (d >= 0)
				continue;

			/* Depth d+3 */
			for (m[2] = 0; m[2] < 18; m[2]++) {
				if (!allowednextmove(m, 3)) {
					m[2] += 2;
					continue;
				}
				cube[2] = move(cube[1], m[2]);
				if (gendata_h48k2_dfs_stop(cube[2], d+3, arg))
					continue;
				gendata_h48k2_mark(cube[2], d+3, arg);
				if (d >= -1)
					continue;

				/* Depth d+4 */
				for (m[3] = 0; m[3] < 18; m[3]++) {
					if (!allowednextmove(m, 4)) {
						m[3] += 2;
						continue;
					}
					cube[3] = move(cube[2], m[3]);
					gendata_h48k2_mark(cube[3], d+4, arg);
				}
			}
		}
	}
}

STATIC_INLINE void
gendata_h48k2_mark(cube_t cube, int8_t depth, h48k2_dfs_arg_t *arg)
{
	uint8_t oldval, newval;
	int64_t coord, fullcoord;

	FOREACH_H48SIM(cube, arg->cocsepdata, arg->selfsim,
		fullcoord = coord_h48(cube, arg->cocsepdata, 11);
		coord = fullcoord >> (int64_t)(11 - arg->h);
		oldval = get_h48_pval(arg->table, coord, arg->k);
		newval = (uint8_t)MAX(depth, 0);
		set_h48_pval(arg->table, coord, arg->k, MIN(oldval, newval));
	)
}

STATIC_INLINE bool
gendata_h48k2_dfs_stop(cube_t cube, uint8_t depth, h48k2_dfs_arg_t *arg)
{
	uint64_t val;
	int64_t coord;
	uint8_t oldval;

	if (arg->h == 0 || arg->h == 11) {
		/* We are in the "real coordinate" case, we can stop
		   if this coordinate has already been visited */
		coord = coord_h48(cube, arg->cocsepdata, arg->h);
		oldval = get_h48_pval(arg->table, coord, arg->k);
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

STATIC_INLINE uint8_t
get_h48_pval(const uint8_t *table, int64_t i, uint8_t k)
{
	return (table[H48_INDEX(i, k)] & H48_MASK(i, k)) >> H48_SHIFT(i, k);
}

STATIC_INLINE void
set_h48_pval(uint8_t *table, int64_t i, uint8_t k, uint8_t val)
{
	table[H48_INDEX(i, k)] = (table[H48_INDEX(i, k)] & (~H48_MASK(i, k)))
	    | (val << H48_SHIFT(i, k));
}

STATIC_INLINE uint8_t
get_h48_bound(cube_t cube, uint32_t cdata, uint8_t h, uint8_t k, uint8_t *table)
{
	int64_t coord;

	coord = coord_h48_edges(cube, COCLASS(cdata), TTREP(cdata), h);
	return get_h48_pval(table, coord, k);
}
