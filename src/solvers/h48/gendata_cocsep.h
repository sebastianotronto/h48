STATIC_INLINE bool get_visited(const uint8_t *, int64_t);
STATIC_INLINE void set_visited(uint8_t *, int64_t);

STATIC size_t gendata_cocsep(void *, uint64_t *, cube_t *);
STATIC uint32_t gendata_cocsep_dfs(cocsep_dfs_arg_t *);
STATIC void getdistribution_cocsep(const uint32_t *, uint64_t [static 21]);

STATIC_INLINE int8_t get_h48_cdata(cube_t, const uint32_t *, uint32_t *);

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
STATIC size_t
gendata_cocsep(void *buf, uint64_t *selfsim, cube_t *rep)
{
	uint32_t *buf32, cc;
	uint16_t n;
	uint8_t i, j, visited[COCSEP_VISITEDSIZE];
	tableinfo_t info;
	cocsep_dfs_arg_t arg;

	if (buf == NULL)
		goto gendata_cocsep_return_size;

	memset(buf, 0xFF, COCSEP_FULLSIZE);
	buf32 = (uint32_t *)((char *)buf + INFOSIZE);
	if (selfsim != NULL)
		memset(selfsim, 0, sizeof(uint64_t) * COCSEP_CLASSES);

	info = (tableinfo_t) {
		.solver = "cocsep data for h48",
		.type = TABLETYPE_SPECIAL,
		.infosize = INFOSIZE,
		.fullsize = COCSEP_FULLSIZE,
		.hash = 0, /* TODO */
		.entries = COCSEP_TABLESIZE,
		.classes = COCSEP_CLASSES,
		.bits = 32,
		.base = 0,
		.maxvalue = 9,
		.next = 0
	};
	arg = (cocsep_dfs_arg_t) {
		.cube = SOLVED_CUBE,
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
		info.distribution[i] = cc;
		LOG("found %" PRIu32 "\n", cc);
	}

	writetableinfo(&info, buf);

	DBG_ASSERT(n == COCSEP_CLASSES, 0,
	    "cocsep: computed %" PRIu16 " symmetry classes, "
	    "expected %zu\n", n, COCSEP_CLASSES);

	LOG("cocsep data computed\n");
	LOG("Symmetry classes: %" PRIu32 "\n", COCSEP_CLASSES);
	LOG("Pruning value distribution:\n");
	for (j = 0; j < 10; j++)
		LOG("%" PRIu8 ":\t%" PRIu32 "\n", j, info.distribution[j]);

gendata_cocsep_return_size:
	return COCSEP_FULLSIZE;
}

STATIC uint32_t
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

	nextarg = *arg;
	nextarg.depth++;
	for (m = 0, cc = 0; m < 18; m++) {
		nextarg.cube = move(arg->cube, m);
		cc += gendata_cocsep_dfs(&nextarg);
	}

	return cc;
}

STATIC void
getdistribution_cocsep(const uint32_t *table, uint64_t distr[static 21])
{
	size_t i;

	memset(distr, 0, 21 * sizeof(uint64_t));

	for (i = 0; i < COCSEP_TABLESIZE; i++)
		distr[CBOUND(table[i])]++;
}

STATIC_INLINE bool
get_visited(const uint8_t *a, int64_t i)
{
	return a[VISITED_IND(i)] & VISITED_MASK(i);
}

STATIC_INLINE void
set_visited(uint8_t *a, int64_t i)
{
	a[VISITED_IND(i)] |= VISITED_MASK(i);
}

STATIC_INLINE int8_t
get_h48_cdata(cube_t cube, const uint32_t *cocsepdata, uint32_t *cdata)
{
	int64_t coord;

	coord = coord_cocsep(cube);
	*cdata = cocsepdata[coord];

	return CBOUND(*cdata);
}
