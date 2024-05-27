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
#define H48_ESIZE(h)          ((_12c4 * _8c4) << (int64_t)(h))

#define ESEP_IND(i)           ((uint32_t)(i) / UINT32_C(8))
#define ESEP_SHIFT(i)         (UINT32_C(4) * ((uint32_t)(i) % UINT32_C(8)))
#define ESEP_MASK(i)          ((_bit_u32(4) - (uint32_t)(1)) << ESEP_SHIFT(i))
#define VISITED_IND(i)        ((uint32_t)(i) / UINT32_C(8))
#define VISITED_MASK(i)       (UINT32_C(1) << ((uint32_t)(i) % UINT32_C(8)))

typedef struct {
	cube_fast_t cube;
	uint8_t depth;
	uint8_t maxdepth;
	uint16_t *n;
	uint32_t *buf32;
	uint8_t *visited;
	uint64_t *selfsim;
	cube_fast_t *rep;
} dfsarg_cocsep_t;

typedef struct {
	uint8_t depth;
	uint8_t h;
	uint32_t *cocsepdata;
	uint32_t *buf32;
	uint64_t *selfsim;
	cube_fast_t *crep;
} bfsarg_esep_t;

_static_inline int64_t coord_h48(cube_fast_t, const uint32_t *, uint8_t);
_static_inline int64_t coord_h48_edges(cube_fast_t, int64_t, uint8_t, uint8_t);
_static_inline cube_fast_t invcoord_h48(int64_t, const cube_fast_t *, uint8_t);

_static size_t gendata_cocsep(void *, uint64_t *, cube_fast_t *);
_static uint32_t gendata_cocsep_dfs(dfsarg_cocsep_t *);
_static size_t gendata_h48(void *, uint8_t, uint8_t);
_static uint64_t gendata_esep_bfs(bfsarg_esep_t *);

_static_inline bool get_visited(const uint8_t *, int64_t);
_static_inline void set_visited(uint8_t *, int64_t);
_static_inline uint8_t get_esep_pval(const uint32_t *, int64_t);
_static_inline void set_esep_pval(uint32_t *, int64_t, uint8_t);

_static_inline int64_t
coord_h48(cube_fast_t c, const uint32_t *cocsepdata, uint8_t h)
{
	int64_t cocsep, coclass;
	uint32_t data;
	uint8_t ttrep;

	DBG_ASSERT(h <= 11, -1, "coord_h48: h must be between 0 and 11\n");

	cocsep = coord_fast_cocsep(c);
	data = cocsepdata[cocsep];
	coclass = (int64_t)COCLASS(data);
	ttrep = (int64_t)TTREP(data);

	return coord_h48_edges(c, coclass, ttrep, h);
}

_static_inline int64_t
coord_h48_edges(cube_fast_t c, int64_t coclass, uint8_t t, uint8_t h)
{
	cube_fast_t d;
	int64_t esep, eo, edges;

	d = transform_edges(c, t);
	esep = coord_fast_esep(d);
	eo = coord_fast_eo(d);
	edges = (esep << (int64_t)h) + (eo >> (11 - (int64_t)h));

	return coclass * H48_ESIZE(h) + edges;
}

/*
This function does not necessarily return a cube whose coordinate is
the given value, because it works up to symmetry. This means that the
returned cube is a transformed cube of one that gives the correct value.
*/
_static_inline cube_fast_t
invcoord_h48(int64_t i, const cube_fast_t *crep, uint8_t h) {
	cube_fast_t ret;
	int64_t hh, coclass, ee, esep, eo;

	DBG_ASSERT(h <= 11, cubetofast(zero),
	    "invcoord_h48: h must be between 0 and 11\n");

	hh = (int64_t)h;
	coclass = i / H48_ESIZE(h);
	ee = i % H48_ESIZE(h);
	esep = ee >> hh;
	eo = (ee & ((1 << hh) - 1)) << (11 - hh);

	ret = invcoord_fast_esep(esep);
	copy_corners_fast(&ret, crep[coclass]);
	set_eo_fast(&ret, eo);

	return ret;
}

/*
Each element of the cocsep table is a uint32_t used as follows:
  - Lowest 8-bit block: pruning value
  - Second-lower 8-bit block: "ttrep" (transformation to representative)
  - Top 16-bit block: symcoord value
After the data as described above, more auxiliary information is appended:
  - A uint32_t representing the number of symmetry classes
  - A uint32_t representing the highest value of the pruning table
  - One uint32_t for each "line" of the pruning table, representing the number
    of positions having that pruning value.
*/
_static size_t
gendata_cocsep(void *buf, uint64_t *selfsim, cube_fast_t *rep)
{
	uint32_t *buf32, *info, cc;
	uint16_t n;
	uint8_t i, j, visited[COCSEP_VISITEDSIZE];
	dfsarg_cocsep_t arg;

	buf32 = (uint32_t *)buf;
	info = buf32 + COCSEP_TABLESIZE;
	memset(buf32, 0xFF, sizeof(uint32_t) * COCSEP_TABLESIZE);
	memset(selfsim, 0, sizeof(uint64_t) * COCSEP_CLASSES);

	arg = (dfsarg_cocsep_t) {
		.cube = cubetofast(solvedcube()),
		.n = &n,
		.buf32 = buf32,
		.visited = visited,
		.selfsim = selfsim,
		.rep = rep
	};
	for (i = 0, n = 0, cc = 0; i < 10; i++) {
		DBG_LOG("cocsep: generating depth %" PRIu8 "\n", i);
		memset(visited, 0, COCSEP_VISITEDSIZE);
		arg.depth = 0;
		arg.maxdepth = i;
		cc = gendata_cocsep_dfs(&arg);
		info[i+2] = cc;
		DBG_LOG("found %" PRIu32 "\n", cc);
	}

	info[0] = (uint32_t)n;
	info[1] = 9; /* Known max pruning value */
	DBG_ASSERT(n == COCSEP_CLASSES, 0,
	    "cocsep: computed %" PRIu16 " symmetry classes, "
	    "expected %zu\n", n, COCSEP_CLASSES);

	DBG_LOG("cocsep data computed\n");
	DBG_LOG("Symmetry classes: %" PRIu32 "\n", info[0]);
	DBG_LOG("Maximum pruning value: %" PRIu32 "\n", info[1]);
	DBG_LOG("Pruning value distribution:\n");
	for (j = 0; j < 10; j++)
		DBG_LOG("%" PRIu8 ":\t%" PRIu32 "\n", j, info[j+2]);

	return COCSEP_FULLSIZE;
}

_static uint32_t
gendata_cocsep_dfs(dfsarg_cocsep_t *arg)
{
	uint8_t m, tinv;
	uint32_t cc, class, ttrep, depth, olddepth;
	uint64_t t, is;
	int64_t i, j;
	cube_fast_t d;
	dfsarg_cocsep_t nextarg;

	i = coord_fast_cocsep(arg->cube);
	olddepth = (uint8_t)(arg->buf32[i] & 0xFF);
	if (olddepth < arg->depth || get_visited(arg->visited, i))
		return 0;
	set_visited(arg->visited, i);

	if (arg->depth == arg->maxdepth) {
		if ((arg->buf32[i] & 0xFF) != 0xFF)
			return 0;

		for (t = 0, cc = 0; t < 48; t++) {
			d = transform_corners(arg->cube, t);
			j = coord_fast_cocsep(d);
			is = (i == j);
			arg->selfsim[*arg->n] |= is << t;
			set_visited(arg->visited, j);
			tinv = inverse_trans(t);
			olddepth = (uint8_t)(arg->buf32[j] & 0xFF);
			cc += olddepth == 0xFF;

			class = (uint32_t)(*arg->n) << 16;
			ttrep = (uint32_t)tinv << 8;
			depth = (uint32_t)arg->depth;
			arg->buf32[j] = class | ttrep | depth;
		}
		arg->rep[*arg->n] = arg->cube;
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

/*
TODO description
generating fixed table with h=0, k=4
*/
_static size_t
gendata_h48(void *buf, uint8_t h, uint8_t maxdepth)
{
	const int k = 4; /* TODO: other cases? */
	uint32_t j, *buf32, *info, *cocsepdata;
	bfsarg_esep_t arg;
	int64_t sc, cc, tot, esep_max;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_fast_t crep[COCSEP_CLASSES];
	size_t cocsepsize, infosize;

	esep_max = (int64_t)ESEP_MAX(h);
	cocsepsize = gendata_cocsep(buf, selfsim, crep);
	cocsepdata = (uint32_t *)buf;
	buf32 = cocsepdata + cocsepsize / 4;
	info = buf32 + (ESEP_TABLESIZE(h, k) / sizeof(uint32_t));
	memset(buf32, 0xFF, ESEP_TABLESIZE(h, k));

	sc = coord_h48(cubetofast(solved), cocsepdata, h);
	set_esep_pval(buf32, sc, 0);
	info[1] = 1;
	arg = (bfsarg_esep_t) {
		.h = h,
		.cocsepdata = cocsepdata,
		.buf32 = buf32,
		.crep = crep,
		.selfsim = selfsim
	};
	for (
		tot = 1, arg.depth = 1, cc = 0;
		tot < esep_max && arg.depth <= maxdepth;
		arg.depth++
	) {
		DBG_LOG("esep: generating depth %" PRIu8 "\n", arg.depth);
		cc = gendata_esep_bfs(&arg);
		tot += cc;
		info[arg.depth+1] = cc;
		DBG_LOG("found %" PRIu64 "\n", cc);
	}

	info[0] = arg.depth-1;
	infosize = 4 * (size_t)(info[0] + 2);

	DBG_LOG("h48 pruning table computed\n");
	DBG_LOG("Maximum pruning value: %" PRIu32 "\n", info[0]);
	DBG_LOG("Pruning value distribution:\n");
	for (j = 0; j <= info[0]; j++)
		DBG_LOG("%" PRIu8 ":\t%" PRIu32 "\n", j, info[j+1]);

	return cocsepsize + ESEP_TABLESIZE(h, k) + infosize;
}

_static uint64_t
gendata_esep_bfs(bfsarg_esep_t *arg)
{
	uint8_t c, m, x;
	uint32_t cc;
	int64_t i, j, k, t, cocsep_coord, sim, esep_max;
	cube_fast_t cube, moved, transd;

	esep_max = (uint64_t)ESEP_MAX(arg->h);

	for (i = 0, cc = 0; i < esep_max; i++) {
		c = get_esep_pval(arg->buf32, i);
		if (c != arg->depth - 1)
			continue;
		cube = invcoord_h48(i, arg->crep, arg->h);
		for (m = 0; m < 18; m++) {
			/*
			 * TODO: here we can optimize by computing at first
			 * only the corner part of the coordinate, and then
			 * the edge parts for each transformation.
			 */
			moved = move(cube, m);
			j = coord_h48(moved, arg->cocsepdata, arg->h);
			x = get_esep_pval(arg->buf32, j);
			if (x <= arg->depth)
				continue;
			set_esep_pval(arg->buf32, j, arg->depth);
			cc += x != arg->depth;
			cocsep_coord = j / H48_ESIZE(arg->h);
			sim = arg->selfsim[cocsep_coord] >> 1;
			for (t = 1; t < 48 && sim; t++, sim >>= 1) {
				if (!(sim & 1))
					continue;
				transd = transform(moved, t);
				k = coord_h48(transd, arg->cocsepdata, arg->h);
				x = get_esep_pval(arg->buf32, k);
				if (x <= arg->depth)
					continue;
				set_esep_pval(arg->buf32, k, arg->depth);
				cc += x != arg->depth;
			}
		}
	}

	return cc;
}

_static_inline bool get_visited(const uint8_t *a, int64_t i)
{
	return a[VISITED_IND(i)] & VISITED_MASK(i);
}

_static_inline void set_visited(uint8_t *a, int64_t i)
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
