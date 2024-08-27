#define COCSEP_CLASSES        ((size_t)3393)
#define COCSEP_TABLESIZE      ((size_t)_3p7 << (size_t)7)
#define COCSEP_VISITEDSIZE    ((COCSEP_TABLESIZE + (size_t)7) / (size_t)8)
#define COCSEP_FULLSIZE       ((size_t)4 * (COCSEP_TABLESIZE + (size_t)12))

#define VISITED_IND(i)        ((uint32_t)(i) / UINT32_C(8))
#define VISITED_MASK(i)       (UINT32_C(1) << ((uint32_t)(i) % UINT32_C(8)))

#define CBOUND_MASK           UINT32_C(0xFF)
#define CBOUND(x)             ((x) & CBOUND_MASK)

typedef struct {
	cube_t cube;
	uint8_t depth;
	uint8_t maxdepth;
	uint16_t *n;
	uint32_t *buf32;
	uint8_t *visited;
	uint64_t *selfsim;
	cube_t *rep;
} cocsep_dfs_arg_t;

_static_inline bool get_visited(const uint8_t *, int64_t);
_static_inline void set_visited(uint8_t *, int64_t);

_static size_t gendata_cocsep(void *, uint64_t *, cube_t *);
_static uint32_t gendata_cocsep_dfs(cocsep_dfs_arg_t *);

_static_inline int8_t get_h48_cdata(cube_t, uint32_t *, uint32_t *);

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
	cocsep_dfs_arg_t arg;

	if (buf == NULL)
		goto gendata_cocsep_return_size;

	buf32 = (uint32_t *)buf;
	info = buf32 + COCSEP_TABLESIZE;
	memset(buf32, 0xFF, sizeof(uint32_t) * COCSEP_TABLESIZE);
	if (selfsim != NULL)
		memset(selfsim, 0, sizeof(uint64_t) * COCSEP_CLASSES);

	arg = (cocsep_dfs_arg_t) {
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
gendata_cocsep_dfs(cocsep_dfs_arg_t *arg)
{
	uint8_t m;
	uint32_t cc, class, ttrep, depth, olddepth, tinv;
	uint64_t t;
	int64_t i, j;
	cube_t d;
	cocsep_dfs_arg_t nextarg;

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

	memcpy(&nextarg, arg, sizeof(cocsep_dfs_arg_t));
	nextarg.depth++;
	for (m = 0, cc = 0; m < 18; m++) {
		nextarg.cube = move(arg->cube, m);
		cc += gendata_cocsep_dfs(&nextarg);
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

_static_inline int8_t
get_h48_cdata(cube_t cube, uint32_t *cocsepdata, uint32_t *cdata)
{
	int64_t coord;

	coord = coord_cocsep(cube);
	*cdata = cocsepdata[coord];

	return CBOUND(*cdata);
}
