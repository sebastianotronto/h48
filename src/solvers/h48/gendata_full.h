#define ESEP_NOEO             (COCSEP_CLASSES * (size_t)_12c4 * (size_t)_8c4)
#define ESEP_MAX(h)           (ESEP_NOEO << (size_t)(h))
#define ESEP_TABLESIZE(h, k)  (ESEP_MAX((h)) / ((size_t)8 / (size_t)(k)))

#define ESEP_IND(i)           ((uint32_t)(i) / UINT32_C(8))
#define ESEP_SHIFT(i)         (UINT32_C(4) * ((uint32_t)(i) % UINT32_C(8)))
#define ESEP_MASK(i)          ((_bit_u32(4) - (uint32_t)(1)) << ESEP_SHIFT(i))

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
	void * buf;
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
} bfsarg_esep_t;

_static_inline uint8_t get_esep_pval(const uint32_t *, int64_t);
_static_inline void set_esep_pval(uint32_t *, int64_t, uint8_t);

_static uint64_t gen_h48short(gendata_h48short_arg_t *);
_static size_t gendata_h48(gendata_h48_arg_t *);
_static size_t gendata_h48h0k4(void *, uint8_t);
_static int64_t gendata_h48h0k4_bfs(bfsarg_esep_t *);
_static int64_t gendata_h48h0k4_bfs_fromdone(bfsarg_esep_t *);
_static int64_t gendata_h48h0k4_bfs_fromnew(bfsarg_esep_t *);
_static size_t gendata_h48k2(void *, uint8_t, uint8_t);

_static_inline int8_t get_h48_bound(cube_t, uint32_t, uint8_t, uint32_t *);

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
	if (arg->h == 0 && arg->k == 4) {
		return gendata_h48h0k4(arg->buf, arg->maxdepth);
	} else if (arg->k == 2) {
		return gendata_h48k2(arg->buf, arg->h, arg->maxdepth);
	}

	LOG("Cannot generate data for h = %" PRIu8 " and k = %" PRIu8
	    " (not implemented yet)\n", arg->h, arg->k);
}

/*
TODO description
generating fixed table with h=0, k=4
*/
_static size_t
gendata_h48h0k4(void *buf, uint8_t maxdepth)
{
	uint32_t j, *buf32, *info, *cocsepdata;
	bfsarg_esep_t arg;
	int64_t sc, cc, esep_max;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t crep[COCSEP_CLASSES];
	size_t cocsepsize, infosize;

	if (buf == NULL)
		goto gendata_h48h0k4_return_size;

	/* TODO: move info at start of tables (all tables!) */
	cocsepsize = gendata_cocsep(buf, selfsim, crep);
	infosize = 88;

	esep_max = (int64_t)ESEP_MAX(0);
	cocsepdata = (uint32_t *)buf;
	buf32 = cocsepdata + cocsepsize / 4;
	info = buf32 + (ESEP_TABLESIZE(0, 4) / sizeof(uint32_t));
	memset(buf32, 0xFF, ESEP_TABLESIZE(0, 4));

	sc = coord_h48(solved, cocsepdata, 0);
	set_esep_pval(buf32, sc, 0);
	info[1] = 1;
	arg = (bfsarg_esep_t) {
		.cocsepdata = cocsepdata,
		.buf32 = buf32,
		.selfsim = selfsim,
		.crep = crep
	};
	for (
		arg.done = 1, arg.depth = 1, cc = 0;
		arg.done < esep_max && arg.depth <= maxdepth;
		arg.depth++
	) {
		LOG("esep: generating depth %" PRIu8 "\n", arg.depth);
		cc = gendata_h48h0k4_bfs(&arg);
		arg.done += cc;
		info[arg.depth+1] = cc;
		LOG("found %" PRId64 "\n", cc);
	}

	info[0] = arg.depth-1;

	LOG("h48 pruning table computed\n");
	LOG("Maximum pruning value: %" PRIu32 "\n", info[0]);
	LOG("Pruning value distribution:\n");
	for (j = 0; j <= info[0]; j++)
		LOG("%" PRIu8 ":\t%" PRIu32 "\n", j, info[j+1]);

gendata_h48h0k4_return_size:
	return cocsepsize + ESEP_TABLESIZE(0, 4) + infosize;
}

_static int64_t
gendata_h48h0k4_bfs(bfsarg_esep_t *arg)
{
	const uint8_t breakpoint = 10; /* Hand-picked optimal */

	if (arg->depth < breakpoint)
		return gendata_h48h0k4_bfs_fromdone(arg);
	else
		return gendata_h48h0k4_bfs_fromnew(arg);
}

_static int64_t
gendata_h48h0k4_bfs_fromdone(bfsarg_esep_t *arg)
{
	uint8_t c, m, x;
	uint32_t cc;
	int64_t i, j, k;
	cube_t cube, moved;

	for (i = 0, cc = 0; i < (int64_t)ESEP_MAX(0); i++) {
		c = get_esep_pval(arg->buf32, i);
		if (c != arg->depth - 1)
			continue;
		cube = invcoord_h48(i, arg->crep, 0);
		for (m = 0; m < 18; m++) {
			moved = move(cube, m);
			j = coord_h48(moved, arg->cocsepdata, 0);
			if (get_esep_pval(arg->buf32, j) <= arg->depth)
				continue;
			_foreach_h48sim(moved, arg->cocsepdata, arg->selfsim, 0,
				k = coord_h48(moved, arg->cocsepdata, 0);
				x = get_esep_pval(arg->buf32, k);
				set_esep_pval(arg->buf32, k, arg->depth);
				cc += x != arg->depth;
			)
		}
	}

	return cc;
}

_static int64_t
gendata_h48h0k4_bfs_fromnew(bfsarg_esep_t *arg)
{
	uint8_t c, m, x;
	uint32_t cc;
	int64_t i, j;
	cube_t cube, moved;

	for (i = 0, cc = 0; i < (int64_t)ESEP_MAX(0); i++) {
		c = get_esep_pval(arg->buf32, i);
		if (c != 0xF)
			continue;
		cube = invcoord_h48(i, arg->crep, 0);
		for (m = 0; m < 18; m++) {
			moved = move(cube, m);
			j = coord_h48(moved, arg->cocsepdata, 0);
			x = get_esep_pval(arg->buf32, j);
			if (x >= arg->depth)
				continue;
			_foreach_h48sim(cube, arg->cocsepdata, arg->selfsim, 0,
				j = coord_h48(cube, arg->cocsepdata, 0);
				x = get_esep_pval(arg->buf32, j);
				set_esep_pval(arg->buf32, j, arg->depth);
				cc += x == 0xF;
			)
			break; /* Enough to find one, skip the rest */
		}
	}

	return cc;
}

_static size_t
gendata_h48k2(void *buf, uint8_t h, uint8_t maxdepth)
{
	static uint64_t capacity = 10000019; /* First prime after 1e8 */
	static uint64_t randomizer = 10000079; /* Second prime after 1e8 */
	static uint8_t base[] = {
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

	uint64_t nshort;
	uint32_t *buf32, *info, *cocsepdata;
	h48map_t depth8cubes;
	gendata_h48short_arg_t shortarg;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t crep[COCSEP_CLASSES];
	size_t cocsepsize, infosize;

	DBG_ASSERT(base[h] == 8, 0, "Only implemented for h <= 3 (base 8)\n");

	if (buf == NULL)
		goto gendata_h48k2_return_size;

	cocsepdata = (uint32_t *)buf;
	cocsepsize = gendata_cocsep(buf, selfsim, crep);
	infosize = 88;

	h48map_create(&depth8cubes, capacity, randomizer);
	shortarg = (gendata_h48short_arg_t) {
		.maxdepth = 8,
		.cocsepdata = cocsepdata,
		.crep = crep,
		.selfsim = selfsim,
		.map = &depth8cubes
	};

	nshort = gen_h48short(&shortarg);
	LOG("%" PRIu64 "\n", nshort);

	h48map_destroy(&depth8cubes);

gendata_h48k2_return_size:
	return cocsepsize + ESEP_TABLESIZE(h, 2) + infosize;
}

_static_inline uint8_t
get_esep_pval(const uint32_t *buf32, int64_t i)
{
	return (buf32[ESEP_IND(i)] & ESEP_MASK(i)) >> ESEP_SHIFT(i);
}

_static_inline void
set_esep_pval(uint32_t *buf32, int64_t i, uint8_t val)
{
	buf32[ESEP_IND(i)] =
	    (buf32[ESEP_IND(i)] & (~ESEP_MASK(i))) | (val << ESEP_SHIFT(i));
}

_static_inline int8_t
get_h48_bound(cube_t cube, uint32_t cdata, uint8_t h, uint32_t *h48data)
{
	int64_t coord;

	coord = coord_h48_edges(cube, COCLASS(cdata), TTREP(cdata), h);
	return get_esep_pval(h48data, coord);
}
