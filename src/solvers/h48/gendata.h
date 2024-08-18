#define COCSEP_CLASSES        ((size_t)3393)
#define COCSEP_TABLESIZE      ((size_t)_3p7 << (size_t)7)
#define COCSEP_VISITEDSIZE    ((COCSEP_TABLESIZE + (size_t)7) / (size_t)8)
#define COCSEP_FULLSIZE       ((size_t)4 * (COCSEP_TABLESIZE + (size_t)12))

#define ESEP_NOEO             (COCSEP_CLASSES * (size_t)_12c4 * (size_t)_8c4)
#define ESEP_MAX(h)           (ESEP_NOEO << (size_t)(h))
#define ESEP_TABLESIZE(h, k)  (ESEP_MAX((h)) / ((size_t)8 / (size_t)(k)))

#define ESEP_IND(i)           ((uint32_t)(i) / UINT32_C(8))
#define ESEP_SHIFT(i)         (UINT32_C(4) * ((uint32_t)(i) % UINT32_C(8)))
#define ESEP_MASK(i)          ((_bit_u32(4) - (uint32_t)(1)) << ESEP_SHIFT(i))
#define VISITED_IND(i)        ((uint32_t)(i) / UINT32_C(8))
#define VISITED_MASK(i)       (UINT32_C(1) << ((uint32_t)(i) % UINT32_C(8)))

#define CBOUND_MASK           UINT32_C(0xFF)
#define CBOUND(x)             ((x) & CBOUND_MASK)

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
	cube_t cube;
	uint8_t depth;
	uint8_t maxdepth;
	uint16_t *n;
	uint32_t *buf32;
	uint8_t *visited;
	uint64_t *selfsim;
	cube_t *rep;
} dfsarg_cocsep_t;

/* TODO keep or not? */
typedef struct {
	cube_t cube;
	int8_t nmoves;
	int8_t depth;
	uint8_t moves[MAXLEN];
	uint32_t *cocsepdata;
	h48map_t *visited;
} dfsarg_genh48set_t;

typedef struct {
	uint8_t depth;
	uint32_t *cocsepdata;
	uint32_t *buf32;
	uint64_t *selfsim;
	int64_t done;
	cube_t *crep;
} bfsarg_esep_t;

_static_inline bool get_visited(const uint8_t *, int64_t);
_static_inline void set_visited(uint8_t *, int64_t);
_static_inline uint8_t get_esep_pval(const uint32_t *, int64_t);
_static_inline void set_esep_pval(uint32_t *, int64_t, uint8_t);

_static size_t gendata_cocsep(void *, uint64_t *, cube_t *);
_static uint32_t gendata_cocsep_dfs(dfsarg_cocsep_t *);
_static uint64_t gen_h48short(
    uint8_t, const uint32_t *, const cube_t *, const uint64_t *, h48map_t *);
_static size_t gendata_h48h0k4(void *, uint8_t);
_static int64_t gendata_h48h0k4_bfs(bfsarg_esep_t *);
_static int64_t gendata_h48h0k4_bfs_fromdone(bfsarg_esep_t *);
_static int64_t gendata_h48h0k4_bfs_fromnew(bfsarg_esep_t *);

_static_inline int8_t get_h48_cdata(cube_t, uint32_t *, uint32_t *);
_static_inline int8_t get_h48_bound(cube_t, uint32_t, uint8_t, uint32_t *);

/*
Each element of the cocsep table is a uint32_t used as follows:
  - Lowest 8-bit block: pruning value
  - Second-lowest 8-bit block: "ttrep" (transformation to representative)
  - Top 16-bit block: symcoord value
After the data as described above, more auxiliary information is appended:
  - A uint32_t representing the number of symmetry classes
  - A uint32_t representing the highest value of the pruning table
  - One uint32_t for each "line" of the pruning table, representing the number
    of positions having that pruning value.
*/
_static size_t
gendata_cocsep(void *buf, uint64_t *selfsim, cube_t *rep)
{
	uint32_t *buf32, *info, cc;
	uint16_t n;
	uint8_t i, j, visited[COCSEP_VISITEDSIZE];
	dfsarg_cocsep_t arg;

	if (buf == NULL)
		goto gendata_cocsep_return_size;

	buf32 = (uint32_t *)buf;
	info = buf32 + COCSEP_TABLESIZE;
	memset(buf32, 0xFF, sizeof(uint32_t) * COCSEP_TABLESIZE);
	if (selfsim != NULL)
		memset(selfsim, 0, sizeof(uint64_t) * COCSEP_CLASSES);

	arg = (dfsarg_cocsep_t) {
		.cube = solved,
		.n = &n,
		.buf32 = buf32,
		.visited = visited,
		.selfsim = selfsim,
		.rep = rep
	};
	for (i = 0, n = 0, cc = 0; i < 10; i++) {
		LOG("cocsep: generating depth %" PRIu8 "\n", i);
		memset(visited, 0, COCSEP_VISITEDSIZE);
		arg.depth = 0;
		arg.maxdepth = i;
		cc = gendata_cocsep_dfs(&arg);
		info[i+2] = cc;
		LOG("found %" PRIu32 "\n", cc);
	}

	info[0] = (uint32_t)n;
	info[1] = 9; /* Known max pruning value */
	DBG_ASSERT(n == COCSEP_CLASSES, 0,
	    "cocsep: computed %" PRIu16 " symmetry classes, "
	    "expected %zu\n", n, COCSEP_CLASSES);

	LOG("cocsep data computed\n");
	LOG("Symmetry classes: %" PRIu32 "\n", info[0]);
	LOG("Maximum pruning value: %" PRIu32 "\n", info[1]);
	LOG("Pruning value distribution:\n");
	for (j = 0; j < 10; j++)
		LOG("%" PRIu8 ":\t%" PRIu32 "\n", j, info[j+2]);

gendata_cocsep_return_size:
	return COCSEP_FULLSIZE;
}

_static uint32_t
gendata_cocsep_dfs(dfsarg_cocsep_t *arg)
{
	uint8_t m;
	uint32_t cc, class, ttrep, depth, olddepth, tinv;
	uint64_t t;
	int64_t i, j;
	cube_t d;
	dfsarg_cocsep_t nextarg;

	i = coord_cocsep(arg->cube);
	olddepth = (uint8_t)(arg->buf32[i] & 0xFF);
	if (olddepth < arg->depth || get_visited(arg->visited, i))
		return 0;
	set_visited(arg->visited, i);

	if (arg->depth == arg->maxdepth) {
		if ((arg->buf32[i] & 0xFF) != 0xFF)
			return 0;

		if (arg->rep != NULL)
			arg->rep[*arg->n] = arg->cube;
		for (t = 0, cc = 0; t < 48; t++) {
			d = transform_corners(arg->cube, t);
			j = coord_cocsep(d);
			if (i == j && arg->selfsim != NULL)
				arg->selfsim[*arg->n] |= UINT64_C(1) << t;
			if (COCLASS(arg->buf32[j]) != UINT32_C(0xFFFF))
				continue;
			set_visited(arg->visited, j);
			tinv = inverse_trans(t);
			olddepth = arg->buf32[j] & 0xFF;
			cc += olddepth == 0xFF;

			class = (uint32_t)(*arg->n) << UINT32_C(16);
			ttrep = (uint32_t)tinv << UINT32_C(8);
			depth = (uint32_t)arg->depth;
			arg->buf32[j] = class | ttrep | depth;
		}
		(*arg->n)++;

		return cc;
	}

	memcpy(&nextarg, arg, sizeof(dfsarg_cocsep_t));
	nextarg.depth++;
	for (m = 0, cc = 0; m < 18; m++) {
		nextarg.cube = move(arg->cube, m);
		cc += gendata_cocsep_dfs(&nextarg);
	}

	return cc;
}

_static uint64_t
gen_h48short(
	uint8_t n,
	const uint32_t *cocsepdata,
	const cube_t *crep,
	const uint64_t *selfsim,
	h48map_t *map
) {
	uint8_t i, m;
	int64_t coord;
	uint64_t j, oldn;
	kvpair_t kv;
	cube_t cube, d;

	cube = solvedcube();
	coord = coord_h48(cube, cocsepdata, 11);
	h48map_insertmin(map, coord, 0);
	oldn = 0;
	LOG("Short h48: generating depth 0\nfound %" PRIu8 "\n", map->n-oldn);
	for (i = 0; i < n; i++) {
		LOG("Short h48: generating depth %" PRIu8 "\n", i+1);
		j = 0;
		oldn = map->n;
		for (kv = h48map_nextkvpair(map, &j);
		     j != map->capacity;
		     kv = h48map_nextkvpair(map, &j)
		) {
			if (kv.val != i)
				continue;
			cube = invcoord_h48(kv.key, crep, 11);
			for (m = 0; m < 18; m++) {
				d = move(cube, m);
				_foreach_h48sim(d, cocsepdata, selfsim, 11,
					coord = coord_h48(d, cocsepdata, 11);
					h48map_insertmin(map, coord, i+1);
				)
			}
		}
		LOG("found %" PRIu8 "\n", map->n-oldn);
	}

	return map->n;
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

	/* TODO: move info at start of tables (all tables!) */
	infosize = 4 * maxdepth;
	cocsepsize = gendata_cocsep(buf, selfsim, crep);
	infosize = 88;

	if (buf == NULL)
		goto gendata_h48h0k4_return_size;

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

_static_inline bool
get_visited(const uint8_t *a, int64_t i)
{
	return a[VISITED_IND(i)] & VISITED_MASK(i);
}

_static_inline void
set_visited(uint8_t *a, int64_t i)
{
	a[VISITED_IND(i)] |= VISITED_MASK(i);
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
get_h48_cdata(cube_t cube, uint32_t *cocsepdata, uint32_t *cdata)
{
	int64_t coord;

	coord = coord_cocsep(cube);
	*cdata = cocsepdata[coord];

	return CBOUND(*cdata);
}

_static_inline int8_t
get_h48_bound(cube_t cube, uint32_t cdata, uint8_t h, uint32_t *h48data)
{
	int64_t coord;

	coord = coord_h48_edges(cube, COCLASS(cdata), TTREP(cdata), h);
	return get_esep_pval(h48data, coord);
}
