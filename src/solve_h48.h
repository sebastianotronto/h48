#define MAP_UNSET             UINT64_C(0xFFFFFFFFFFFFFFFF)
#define MAP_KEYMASK           UINT64_C(0xFFFFFFFFFF)
#define MAP_KEYSHIFT          UINT64_C(40)

#define COCSEP_CLASSES        ((size_t)3393)
#define COCSEP_TABLESIZE      ((size_t)_3p7 << (size_t)7)
#define COCSEP_VISITEDSIZE    ((COCSEP_TABLESIZE + (size_t)7) / (size_t)8)
#define COCSEP_FULLSIZE       ((size_t)4 * (COCSEP_TABLESIZE + (size_t)12))

#define ESEP_NOEO             (COCSEP_CLASSES * (size_t)_12c4 * (size_t)_8c4)
#define ESEP_MAX(h)           (ESEP_NOEO << (size_t)(h))
#define ESEP_TABLESIZE(h, k)  (ESEP_MAX((h)) / ((size_t)8 / (size_t)(k)))

#define COCLASS_MASK          (UINT32_C(0xFFFF) << UINT32_C(16))
#define COCLASS(x)            (((x) & COCLASS_MASK) >> UINT32_C(16))
#define TTREP_MASK            (UINT32_C(0xFF) << UINT32_C(8))
#define TTREP(x)              (((x) & TTREP_MASK) >> UINT32_C(8))
#define CBOUND_MASK           UINT32_C(0xFF)
#define CBOUND(x)             ((x) & CBOUND_MASK)
#define H48_ESIZE(h)          ((_12c4 * _8c4) << (int64_t)(h))

#define ESEP_IND(i)           ((uint32_t)(i) / UINT32_C(8))
#define ESEP_SHIFT(i)         (UINT32_C(4) * ((uint32_t)(i) % UINT32_C(8)))
#define ESEP_MASK(i)          ((_bit_u32(4) - (uint32_t)(1)) << ESEP_SHIFT(i))
#define VISITED_IND(i)        ((uint32_t)(i) / UINT32_C(8))
#define VISITED_MASK(i)       (UINT32_C(1) << ((uint32_t)(i) % UINT32_C(8)))

#define MAX_SOLUTION_LENGTH 20

/*
TODO: This loop other similar h48 coordinates can be improved by only
transforming edges, but we need to compose transformations (i.e. conjugate
_t by _ttrep).
*/
#define _foreach_h48sim(_cube, _cocsepdata, _selfsim, _h, _action) \
	int64_t _cocsep = coord_cocsep(_cube); \
	uint8_t _ttrep = TTREP(_cocsepdata[_cocsep]); \
	int64_t _coclass = COCLASS(_cocsepdata[_cocsep]); \
	cube_t _rep = transform(_cube, _ttrep); \
	uint64_t _sim = _selfsim[_coclass]; \
	for (uint8_t _t = 0; _t < 48 && _sim; _t++, _sim >>= 1) { \
		if (!(_sim & 1)) continue; \
		_cube = transform(_rep, _t); \
		_cube = transform(_cube, inverse_trans(_ttrep)); \
		_action \
	}

typedef struct {
	uint64_t n;
	uint64_t capacity;
	uint64_t randomizer;
	uint64_t *table;
} h48map_t;

typedef struct {
	uint64_t key;
	uint64_t val;
} kvpair_t;

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
	uint8_t moves[MAX_SOLUTION_LENGTH];
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

typedef struct {
	cube_t cube;
	cube_t inverse;
	int8_t nmoves;
	int8_t depth;
	uint8_t moves[MAX_SOLUTION_LENGTH];
	int64_t *nsols;
	int64_t maxsolutions;
	uint8_t h;
	uint32_t *cocsepdata;
	uint32_t *h48data;
	char **nextsol;
} dfsarg_solveh48_t;

typedef struct {
	cube_t cube;
	int8_t nmoves;
	int8_t depth;
	uint8_t moves[MAX_SOLUTION_LENGTH];
	uint32_t *cocsepdata;
	uint32_t *h48data;
	char *s;
} dfsarg_solveh48stats_t;

_static void h48map_create(h48map_t *, uint64_t, uint64_t);
_static void h48map_clear(h48map_t *);
_static void h48map_destroy(h48map_t *);
_static uint64_t h48map_lookup(h48map_t *, uint64_t);
_static void h48map_insertmin(h48map_t *, uint64_t, uint64_t);
_static uint64_t h48map_value(h48map_t *, uint64_t);
_static kvpair_t h48map_nextkvpair(h48map_t *, uint64_t *);

_static_inline int64_t coord_h48(cube_t, const uint32_t *, uint8_t);
_static_inline int64_t coord_h48_edges(cube_t, int64_t, uint8_t, uint8_t);
_static_inline cube_t invcoord_h48(int64_t, const cube_t *, uint8_t);

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

_static void solve_h48_appendsolution(dfsarg_solveh48_t *);
_static_inline int8_t get_h48_cdata(cube_t, uint32_t *, uint32_t *);
_static_inline int8_t get_h48_bound(cube_t, uint32_t, uint8_t, uint32_t *);
_static_inline bool solve_h48_stop(dfsarg_solveh48_t *);
_static int64_t solve_h48_dfs(dfsarg_solveh48_t *);
_static int64_t solve_h48(
    cube_t, int8_t, int8_t, int8_t, uint8_t, const void *, char *);

_static int64_t solve_h48stats_dfs(dfsarg_solveh48stats_t *);
_static int64_t solve_h48stats(cube_t, int8_t, const void *, char [static 12]);

_static void
h48map_create(h48map_t *map, uint64_t capacity, uint64_t randomizer)
{
	map->capacity = capacity;
	map->randomizer = randomizer;

	map->table = malloc(map->capacity * sizeof(int64_t));
	h48map_clear(map);
}

_static void
h48map_clear(h48map_t *map)
{
	memset(map->table, 0xFF, map->capacity * sizeof(uint64_t));
	map->n = 0;
}

_static void
h48map_destroy(h48map_t *map)
{
	free(map->table);
}

_static_inline uint64_t
h48map_lookup(h48map_t *map, uint64_t x)
{
	uint64_t hash, i;

	hash = ((x % map->capacity) * map->randomizer) % map->capacity;
	for (i = hash;
	     map->table[i] != MAP_UNSET && (map->table[i] & MAP_KEYMASK) != x;
	     i = (i+1) % map->capacity
	) ;

	return i;
}

_static_inline void
h48map_insertmin(h48map_t *map, uint64_t key, uint64_t val)
{
	uint64_t i, oldval, min;

	i = h48map_lookup(map, key);
	oldval = map->table[i] >> MAP_KEYSHIFT;
	min = _min(val, oldval);

	map->n += map->table[i] == MAP_UNSET;
	map->table[i] = (key & MAP_KEYMASK) | (min << MAP_KEYSHIFT);
}

_static_inline uint64_t
h48map_value(h48map_t *map, uint64_t key)
{
	return map->table[h48map_lookup(map, key)] >> MAP_KEYSHIFT;
}

_static kvpair_t
h48map_nextkvpair(h48map_t *map, uint64_t *p)
{
	kvpair_t kv;
	uint64_t pair;

	kv.key = MAP_UNSET;
	kv.val = MAP_UNSET;

	DBG_ASSERT(*p < map->capacity, kv,
	    "Error looping over map: given index %" PRIu64 " is out of "
	    "range [0,%" PRIu64 "]", *p, map->capacity);

	for ( ; *p < map->capacity; (*p)++) {
		if (map->table[*p] != MAP_UNSET) {
			pair = map->table[(*p)++];
			kv.key = pair & MAP_KEYMASK;
			kv.val = pair >> MAP_KEYSHIFT;
			return kv;
		}
	}

	return kv;
}

_static_inline int64_t
coord_h48(cube_t c, const uint32_t *cocsepdata, uint8_t h)
{
	int64_t cocsep, coclass;
	uint32_t data;
	uint8_t ttrep;

	DBG_ASSERT(h <= 11, -1, "coord_h48: h must be between 0 and 11\n");

	cocsep = coord_cocsep(c);
	data = cocsepdata[cocsep];
	coclass = (int64_t)COCLASS(data);
	ttrep = (int64_t)TTREP(data);

	return coord_h48_edges(c, coclass, ttrep, h);
}

_static_inline int64_t
coord_h48_edges(cube_t c, int64_t coclass, uint8_t ttrep, uint8_t h)
{
	cube_t d;
	int64_t esep, eo, edges;

	d = transform_edges(c, ttrep);
	esep = coord_esep(d);
	eo = coord_eo(d);
	edges = (esep << 11) + eo;

	return (coclass * H48_ESIZE(11) + edges) >> (11 - (int64_t)h);
}

/*
This function does not necessarily return a cube whose coordinate is
the given value, because it works up to symmetry. This means that the
returned cube is a transformed cube of one that gives the correct value.
*/
_static_inline cube_t
invcoord_h48(int64_t i, const cube_t *crep, uint8_t h)
{
	cube_t ret;
	int64_t hh, coclass, ee, esep, eo;

	DBG_ASSERT(h <= 11, zero,
	    "invcoord_h48: h must be between 0 and 11\n");

	hh = (int64_t)h;
	coclass = i / H48_ESIZE(h);
	ee = i % H48_ESIZE(h);
	esep = ee >> hh;
	eo = (ee & ((1 << hh) - 1)) << (11 - hh);

	ret = invcoord_esep(esep);
	copy_corners(&ret, crep[coclass]);
	set_eo(&ret, eo);

	return ret;
}

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
	if (2 * arg->done < (int64_t)ESEP_MAX(0))
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
#if 0
			cube_t transd;
			int64_t t, cocsep_coord, sim;

			set_esep_pval(arg->buf32, i, arg->depth);
			cc++;
			cocsep_coord = i / H48_ESIZE(0);
			sim = arg->selfsim[cocsep_coord] >> 1;
			for (t = 1; t < 48 && sim; t++) {
				transd = transform(cube, t);
				j = coord_h48(transd, arg->cocsepdata, 0);
				x = get_esep_pval(arg->buf32, j);
				set_esep_pval(arg->buf32, j, arg->depth);
				cc += x == 0xF;
			}
#else
			_foreach_h48sim(cube, arg->cocsepdata, arg->selfsim, 0,
				j = coord_h48(cube, arg->cocsepdata, 0);
				x = get_esep_pval(arg->buf32, j);
				set_esep_pval(arg->buf32, j, arg->depth);
				cc += x == 0xF;
			)
#endif

			break;
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

_static void
solve_h48_appendsolution(dfsarg_solveh48_t *arg)
{
	int strl;

	strl = writemoves(arg->moves, arg->nmoves, *arg->nextsol);
	LOG("Solution found: %s\n", *arg->nextsol);
	*arg->nextsol += strl;
	**arg->nextsol = '\n';
	(*arg->nextsol)++;
	(*arg->nsols)++;
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

_static_inline bool
solve_h48_stop(dfsarg_solveh48_t *arg)
{
	uint32_t data, data_inv;
	int8_t bound;

	bound = get_h48_cdata(arg->cube, arg->cocsepdata, &data);
	if (bound + arg->nmoves > arg->depth)
		return true;

	bound = get_h48_cdata(arg->inverse, arg->cocsepdata, &data_inv);
	if (bound + arg->nmoves > arg->depth)
		return true;

/*
	bound = get_h48_bound(arg->cube, data, arg->h, arg->h48data);
LOG("Using pval %" PRId8 "\n", bound);
	if (bound + arg->nmoves > arg->depth)
		return true;

	bound = get_h48_bound(arg->inverse, data_inv, arg->h, arg->h48data);
	if (bound + arg->nmoves > arg->depth)
		return true;
*/

	return false;
}

_static int64_t
solve_h48_dfs(dfsarg_solveh48_t *arg)
{
	dfsarg_solveh48_t nextarg;
	int64_t ret;
	uint8_t m;

	if (*arg->nsols == arg->maxsolutions)
		return 0;

	if (solve_h48_stop(arg))
		return 0;

	if (issolved(arg->cube)) {
		if (arg->nmoves != arg->depth)
			return 0;
		solve_h48_appendsolution(arg);
		return 1;
	}

	/* TODO: avoid copy, change arg and undo changes after recursion */
	nextarg = *arg;
	nextarg.nmoves = arg->nmoves + 1;
	ret = 0;
	for (m = 0; m < 18; m++) {
		nextarg.moves[arg->nmoves] = m;
		if (!allowednextmove(nextarg.moves, nextarg.nmoves)) {
			/* If a move is not allowed, neither are its 180
			 * and 270 degree variations */
			m += 2;
			continue;
		}
		nextarg.cube = move(arg->cube, m);
		nextarg.inverse = inverse(nextarg.cube); /* TODO: use premove */
		ret += solve_h48_dfs(&nextarg);
	}

	return ret;
}

_static int64_t
solve_h48(
	cube_t cube,
	int8_t minmoves,
	int8_t maxmoves,
	int8_t maxsolutions,
	uint8_t h,
	const void *data,
	char *solutions
)
{
	int64_t nsols;
	dfsarg_solveh48_t arg;

	arg = (dfsarg_solveh48_t) {
		.cube = cube,
		.inverse = inverse(cube),
		.nsols = &nsols,
		.maxsolutions = maxsolutions,
		.h = h,
		.cocsepdata = (uint32_t *)data,
		.h48data = ((uint32_t *)data) + COCSEP_FULLSIZE / 4,
		.nextsol = &solutions
	};

	nsols = 0;
	for (arg.depth = minmoves;
	     arg.depth <= maxmoves && nsols < maxsolutions;
	     arg.depth++)
	{
		LOG("Found %" PRId64 " solutions, searching at depth %"
		    PRId8 "\n", nsols, arg.depth);
		arg.nmoves = 0;
		solve_h48_dfs(&arg);
	}

	return nsols;
}

/*
The h48stats solver computes how many moves it takes to solve to
each of the 12 h48 coordinates, one for each value of h from 0 to 11.
The solutions array is filled with the length of the solutions. The
solution array is therefore not a printable string.
*/
_static int64_t
solve_h48stats_dfs(dfsarg_solveh48stats_t *arg)
{
	const int64_t limit = 11;

	int8_t bound, u;
	uint8_t m;
	uint32_t d;
	int64_t coord, h;
	dfsarg_solveh48stats_t nextarg;

	/* Check cocsep lower bound (corners only) */
	bound = get_h48_cdata(arg->cube, arg->cocsepdata, &d);
	if (bound + arg->nmoves > arg->depth)
		return 0;

	/* Check h48 lower bound for h=0 (esep, but no eo) */
	coord = coord_h48_edges(arg->cube, COCLASS(d), TTREP(d), 0);
	bound = get_esep_pval(arg->h48data, coord);
	if (bound + arg->nmoves > arg->depth)
		return 0;
	
	/* Update all other values, if solved */
	coord = coord_h48_edges(arg->cube, COCLASS(d), TTREP(d), 11);
	for (h = 0; h <= limit; h++) {
		u = coord >> (11-h) == 0 && arg->s[h] == 99;
		arg->s[h] = u * arg->nmoves + (1-u) * arg->s[h];
	}

	if (arg->s[limit] != 99)
		return 0;

	nextarg = *arg;
	nextarg.nmoves = arg->nmoves + 1;
	for (m = 0; m < 18; m++) {
		nextarg.moves[arg->nmoves] = m;
		if (!allowednextmove(nextarg.moves, nextarg.nmoves)) {
			/* If a move is not allowed, neither are its 180
			 * and 270 degree variations */
			m += 2;
			continue;
		}
		nextarg.cube = move(arg->cube, m);
		solve_h48stats_dfs(&nextarg);
	}

	return 0;
}

_static int64_t
solve_h48stats(
	cube_t cube,
	int8_t maxmoves,
	const void *data,
	char solutions[static 12]
)
{
	int i;
	size_t cocsepsize;
	dfsarg_solveh48stats_t arg;

	cocsepsize = gendata_cocsep(NULL, NULL, NULL);

	arg = (dfsarg_solveh48stats_t) {
		.cube = cube,
		.cocsepdata = (uint32_t *)data,
		.h48data = ((uint32_t *)data) + (cocsepsize/4),
		.s = solutions
	};

	for (i = 0; i < 12; i++)
		solutions[i] = (char)99;

	for (arg.depth = 0;
	     arg.depth <= maxmoves && solutions[11] == 99;
	     arg.depth++)
	{
		arg.nmoves = 0;
		solve_h48stats_dfs(&arg);
	}

	return 0;
}
