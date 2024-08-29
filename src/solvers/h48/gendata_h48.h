#define H48_COORDMAX_NOEO    ((int64_t)(COCSEP_CLASSES * _12c4 * _8c4))
#define H48_COORDMAX(h)      ((int64_t)(H48_COORDMAX_NOEO << (int64_t)(h)))
#define H48_TABLESIZE(h, k)  ((size_t)H48_COORDMAX((h)) / ((size_t)8 / (size_t)(k)))

#define H48_COEFF(k)         (UINT32_C(32) / (uint32_t)(k))
#define H48_INDEX(i, k)      ((uint32_t)(i) / H48_COEFF(k))
#define H48_SHIFT(i, k)      ((uint32_t)(k) * ((uint32_t)(i) % H48_COEFF(k)))
#define H48_MASK(i, k)       ((_bit_u32(k) - (uint32_t)(1)) << H48_SHIFT(i, k))

#define MAXLEN 20

/*
TODO: This loop over similar h48 coordinates can be improved by only
transforming edges, but we need to compose transformations (i.e. conjugate
_t by _ttrep).
*/
#define _foreach_h48sim(_cube, _cocsepdata, _selfsim, _h, _action) \
	int64_t _cocsep = coord_cocsep(_cube); \
	uint8_t _ttrep = TTREP(_cocsepdata[_cocsep]); \
	uint8_t _inverse_ttrep = inverse_trans(_ttrep); \
	int64_t _coclass = COCLASS(_cocsepdata[_cocsep]); \
	cube_t _rep = transform(_cube, _ttrep); \
	uint64_t _sim = _selfsim[_coclass]; \
	for (uint8_t _t = 0; _t < 48 && _sim; _t++, _sim >>= 1) { \
		if (!(_sim & 1)) continue; \
		_cube = transform(_rep, _t); \
		_cube = transform(_cube, _inverse_ttrep); \
		_action \
	}

typedef struct {
	uint8_t h;
	uint8_t k;
	uint8_t maxdepth;
	void *buf;
	uint32_t *info;
	uint32_t *cocsepdata;
	uint32_t *h48data;
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
	uint32_t *buf32;
	uint64_t *selfsim;
	int64_t done;
	cube_t *crep;
} h48h0k4_bfs_arg_t;

typedef struct {
	cube_t cube;
	uint8_t moves[4];
	uint8_t h;
	uint8_t k;
	uint8_t base;
	uint8_t depth;
	uint8_t shortdepth;
	uint8_t maxdepth;
	uint32_t *cocsepdata;
	uint32_t *h48data;
	uint64_t *selfsim;
	cube_t *crep;
	h48map_t *shortcubes;
} h48k2_dfs_arg_t;

_static_inline uint8_t get_esep_pval(const uint32_t *, int64_t, uint8_t);
_static_inline void set_esep_pval(uint32_t *, int64_t, uint8_t, uint8_t);

_static uint64_t gen_h48short(gendata_h48short_arg_t *);
_static size_t gendata_h48(gendata_h48_arg_t *);
_static size_t gendata_h48h0k4(gendata_h48_arg_t *);
_static int64_t gendata_h48h0k4_bfs(h48h0k4_bfs_arg_t *);
_static int64_t gendata_h48h0k4_bfs_fromdone(h48h0k4_bfs_arg_t *);
_static int64_t gendata_h48h0k4_bfs_fromnew(h48h0k4_bfs_arg_t *);
_static size_t gendata_h48k2(gendata_h48_arg_t *);
_static void gendata_h48k2_dfs(h48k2_dfs_arg_t *arg);

_static_inline int8_t get_h48_bound(cube_t, uint32_t, uint8_t, uint8_t, uint32_t *);

_static uint64_t
gen_h48short(gendata_h48short_arg_t *arg)
{
	uint8_t i, m;
	int64_t coord;
	uint64_t j, oldn;
	kvpair_t kv;
	cube_t cube, d;

	cube = solvedcube();
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
				_foreach_h48sim(
					d, arg->cocsepdata, arg->selfsim, 11,
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
_static size_t
gendata_h48(gendata_h48_arg_t *arg)
{
	static const size_t infosize = 88; /* TODO: change to e.g. 1024 */

	size_t cocsepsize, h48size;

	/* TODO: move info at the start */
	arg->cocsepdata = (uint32_t *)arg->buf;
	cocsepsize = gendata_cocsep(
	    (void *)arg->cocsepdata, arg->selfsim, arg->crep);
	arg->h48data = arg->cocsepdata + (cocsepsize / sizeof(uint32_t));
	arg->info = arg->h48data +
	    (H48_TABLESIZE(arg->h, arg->k) / sizeof(uint32_t));

	if (arg->buf != NULL)
		memset(arg->h48data, 0xFF, H48_TABLESIZE(arg->h, arg->k));

	if (arg->h == 0 && arg->k == 4) {
		h48size = gendata_h48h0k4(arg);
	} else if (arg->k == 2) {
		h48size = gendata_h48k2(arg);
	} else {
		h48size = 0;
		LOG("Cannot generate data for h = %" PRIu8 " and k = %" PRIu8
		    " (not implemented yet)\n", arg->h, arg->k);
	}

	return infosize + cocsepsize + h48size;
}

/*
TODO description
generating fixed table with h=0, k=4
*/
_static size_t
gendata_h48h0k4(gendata_h48_arg_t *arg)
{
	uint32_t j;
	h48h0k4_bfs_arg_t bfsarg;
	int64_t sc, cc, esep_max;

	if (arg->buf == NULL)
		goto gendata_h48h0k4_return_size;

	esep_max = (int64_t)H48_COORDMAX(0);
	sc = coord_h48(solved, arg->cocsepdata, 0);
	set_esep_pval(arg->h48data, sc, 4, 0);
	arg->info[1] = 1;
	bfsarg = (h48h0k4_bfs_arg_t) {
		.cocsepdata = arg->cocsepdata,
		.buf32 = arg->h48data,
		.selfsim = arg->selfsim,
		.crep = arg->crep
	};
	for (
		bfsarg.done = 1, bfsarg.depth = 1, cc = 0;
		bfsarg.done < esep_max && bfsarg.depth <= arg->maxdepth;
		bfsarg.depth++
	) {
		LOG("esep: generating depth %" PRIu8 "\n", bfsarg.depth);
		cc = gendata_h48h0k4_bfs(&bfsarg);
		bfsarg.done += cc;
		arg->info[bfsarg.depth+1] = cc;
		LOG("found %" PRId64 "\n", cc);
	}

	arg->info[0] = bfsarg.depth-1;

	LOG("h48 pruning table computed\n");
	LOG("Maximum pruning value: %" PRIu32 "\n", arg->info[0]);
	LOG("Pruning value distribution:\n");
	for (j = 0; j <= arg->info[0]; j++)
		LOG("%" PRIu8 ":\t%" PRIu32 "\n", j, arg->info[j+1]);

gendata_h48h0k4_return_size:
	return H48_TABLESIZE(0, 4);
}

_static int64_t
gendata_h48h0k4_bfs(h48h0k4_bfs_arg_t *arg)
{
	const uint8_t breakpoint = 10; /* Hand-picked optimal */

	if (arg->depth < breakpoint)
		return gendata_h48h0k4_bfs_fromdone(arg);
	else
		return gendata_h48h0k4_bfs_fromnew(arg);
}

_static int64_t
gendata_h48h0k4_bfs_fromdone(h48h0k4_bfs_arg_t *arg)
{
	uint8_t c, m, x;
	uint32_t cc;
	int64_t i, j, k;
	cube_t cube, moved;

	for (i = 0, cc = 0; i < (int64_t)H48_COORDMAX(0); i++) {
		c = get_esep_pval(arg->buf32, i, 4);
		if (c != arg->depth - 1)
			continue;
		cube = invcoord_h48(i, arg->crep, 0);
		for (m = 0; m < 18; m++) {
			moved = move(cube, m);
			j = coord_h48(moved, arg->cocsepdata, 0);
			if (get_esep_pval(arg->buf32, j, 4) <= arg->depth)
				continue;
			_foreach_h48sim(moved, arg->cocsepdata, arg->selfsim, 0,
				k = coord_h48(moved, arg->cocsepdata, 0);
				x = get_esep_pval(arg->buf32, k, 4);
				set_esep_pval(arg->buf32, k, 4, arg->depth);
				cc += x != arg->depth;
			)
		}
	}

	return cc;
}

_static int64_t
gendata_h48h0k4_bfs_fromnew(h48h0k4_bfs_arg_t *arg)
{
	uint8_t c, m, x;
	uint32_t cc;
	int64_t i, j;
	cube_t cube, moved;

	for (i = 0, cc = 0; i < (int64_t)H48_COORDMAX(0); i++) {
		c = get_esep_pval(arg->buf32, i, 4);
		if (c != 0xF)
			continue;
		cube = invcoord_h48(i, arg->crep, 0);
		for (m = 0; m < 18; m++) {
			moved = move(cube, m);
			j = coord_h48(moved, arg->cocsepdata, 0);
			x = get_esep_pval(arg->buf32, j, 4);
			if (x >= arg->depth)
				continue;
			_foreach_h48sim(cube, arg->cocsepdata, arg->selfsim, 0,
				j = coord_h48(cube, arg->cocsepdata, 0);
				x = get_esep_pval(arg->buf32, j, 4);
				set_esep_pval(arg->buf32, j, 4, arg->depth);
				cc += x == 0xF;
			)
			break; /* Enough to find one, skip the rest */
		}
	}

	return cc;
}

_static size_t
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
	int64_t j;
	uint64_t nshort, i;
	h48map_t shortcubes;
	kvpair_t kv;
	gendata_h48short_arg_t shortarg;
	h48k2_dfs_arg_t dfsarg;

	DBG_ASSERT(base[arg->h] == 8, 0, "Only implemented for h <= 3 (base 8)\n");

	if (arg->buf == NULL)
		goto gendata_h48k2_return_size;

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

	dfsarg = (h48k2_dfs_arg_t){
		.h = arg->h,
		.k = arg->k,
		.base = base[arg->h],
		.depth = shortdepth,
		.shortdepth = shortdepth,
		.maxdepth = arg->maxdepth,
		.cocsepdata = arg->cocsepdata,
		.h48data = arg->h48data,
		.selfsim = arg->selfsim,
		.crep = arg->crep,
		.shortcubes = &shortcubes
	};

	i = 0;
	for (kv = h48map_nextkvpair(&shortcubes, &i);
	     i != shortcubes.capacity;
	     kv = h48map_nextkvpair(&shortcubes, &i)
	) {
		/* TODO maybe over all sim? */
		dfsarg.cube = invcoord_h48(kv.key, arg->crep, 11);
		gendata_h48k2_dfs(&dfsarg);
	}

	h48map_destroy(&shortcubes);

	/* TODO: move info update to dfs? */
	memset(arg->info, 0, 5 * sizeof(arg->info[0]));
	arg->info[0] = base[arg->k];
	for (j = 0; j < H48_COORDMAX(arg->h); j++) {
		t = get_esep_pval(arg->h48data, j, 2);
		arg->info[1 + t]++;
	}

gendata_h48k2_return_size:
	return H48_TABLESIZE(arg->h, 2);
}

_static void
gendata_h48k2_dfs(h48k2_dfs_arg_t *arg)
{
	uint8_t nmoves, oldval, newval;
	uint64_t val;
	int64_t coord, fullcoord;
	h48k2_dfs_arg_t nextarg;
	uint8_t m;

	fullcoord = coord_h48(arg->cube, arg->cocsepdata, 11);
	coord = fullcoord >> (int64_t)(11 - arg->h);

	val = h48map_value(arg->shortcubes, fullcoord);

	if (arg->depth >= arg->base && arg->depth <= arg->base + 2) {
		oldval = get_esep_pval(arg->h48data, coord, arg->k);
		newval = _min(oldval, arg->depth - arg->base);
		set_esep_pval(arg->h48data, coord, arg->k, newval);
	}

	if ((val < arg->shortdepth) ||
	    (arg->depth > arg->shortdepth && val != MAP_UNSET) ||
	    (arg->depth >= arg->maxdepth || arg->depth >= arg->base + 2))
		return;

	/* TODO: avoid copy, change arg and undo changes after recursion */
	nextarg = *arg;
	nextarg.depth = arg->depth + 1;
	nmoves = nextarg.depth - arg->shortdepth;
	for (m = 0; m < 18; m++) {
		nextarg.moves[nmoves - 1] = m;
		if (!allowednextmove(nextarg.moves, nmoves)) {
			m += 2;
			continue;
		}
		nextarg.cube = move(arg->cube, m);
		gendata_h48k2_dfs(&nextarg);
	}
}

_static_inline uint8_t
get_esep_pval(const uint32_t *buf32, int64_t i, uint8_t k)
{
	return (buf32[H48_INDEX(i, k)] & H48_MASK(i, k)) >> H48_SHIFT(i, k);
}

_static_inline void
set_esep_pval(uint32_t *buf32, int64_t i, uint8_t k, uint8_t val)
{
	buf32[H48_INDEX(i, k)] = (buf32[H48_INDEX(i, k)] & (~H48_MASK(i, k)))
	    | (val << H48_SHIFT(i, k));
}

_static_inline int8_t
get_h48_bound(cube_t cube, uint32_t cdata, uint8_t h, uint8_t k, uint32_t *h48data)
{
	int64_t coord;

	coord = coord_h48_edges(cube, COCLASS(cdata), TTREP(cdata), h);
	return get_esep_pval(h48data, coord, k);
}
