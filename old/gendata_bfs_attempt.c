_static size_t gendata_cocsep(void *);
_static uint32_t dfs_cocsep(cube_fast_t, uint8_t, uint8_t, uint32_t *);

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
gendata_cocsep(void *buf)
{
	uint32_t *buf32, cc;
	uint64_t i64;
	uint16_t n;
	uint8_t i, j;
	size_t tablesize;

	tablesize = _3p7 << 7U;

	buf32 = (uint32_t *)buf;
	memset(buf32, 0xFFU, 4*tablesize);
	memset(buf32 + tablesize, 0, 21*4);

/* New impl BFS

	uint32_t nold = 0, nnew = 0;
	uint64_t coord;
	uint8_t m, olddepth;
	cube_fast_t c, d, oldlevel[100000], newlevel[100000];
	newlevel[0] = cubetofast(solvedcube());
	nnew = 1;
	buf32[coord_fast_cocsep(newlevel[0])] = UFr << 8U;
	n = 1;
	DBG_LOG("gendata_cocsep: found 1 position at depth 0\n");
	for (i = 1; i < 10; i++) {
		DBG_LOG("gendata_cocsep: generating depth %" PRIu8 "\n", i);
		memcpy(oldlevel, newlevel, nnew * sizeof(cube_fast_t));
		nold = nnew;
		nnew = 0;
		for (j = 0; j < nold; j++) {
			_foreach_move(m, oldlevel[j], newlevel[nnew],
				coord = coord_fast_cocsep(newlevel[nnew]);
				olddepth = buf32[coord] & 0xFFU;
				buf32[coord] = i;
				nnew += olddepth > i;
			)
		}
		DBG_LOG("found %" PRIu32 "\n", nnew);
	}

End new impl BFS */

	/* Pruning values */
	buf32[tablesize+1] = 9U; /* Known max pruning value */
	for (i = 0, cc = 0; i < 10; i++) {
		DBG_LOG("gendata_cocsep: generating depth %" PRIu8 "\n", i);
		cc = dfs_cocsep(cubetofast(solvedcube()), 0, i, buf32);
		buf32[tablesize+i+2] = cc;
		DBG_LOG("found %" PRIu32 "\n", cc);
	}

	/* Symmetries */
	for (i64 = 0, n = 0; i64 < tablesize; i64++) {
	}
	buf32[tablesize] = (uint32_t)n;

	DBG_LOG("cocsep data computed, %" PRIu32 " symmetry classes\n", n);
	DBG_LOG("Maximum pruning value: %" PRIu32 "\n", buf32[tablesize+1]);
	DBG_LOG("Pruning value distribution:\n");
	for (j = 0; j < 10; j++)
		DBG_LOG("%" PRIu8 ":\t%" PRIu32 "\n", j, buf32[tablesize+j+2]);

	return 4*(tablesize + 11);
}

_static uint32_t
dfs_cocsep(cube_fast_t c, uint8_t depth, uint8_t maxdepth, uint32_t *buf32)
{
	uint8_t m, olddepth;
	uint32_t update, cc;
	uint64_t i;
	cube_fast_t d;

	i = coord_fast_cocsep(c);
	olddepth = (uint8_t)(buf32[i] & 0xFFU);
	if (olddepth < depth)
		return 0;

	if (depth == maxdepth) {
		update = (buf32[i] & 0xFFU) == 0xFFU;
		buf32[i] = depth;
		return update;
	}

	cc = 0;
	_foreach_move(m, c, d,
		cc += dfs_cocsep(d, depth+1, maxdepth, buf32);
	)

	return cc;
}

/*
_static uint32_t
dfs_cocsep(
	cube_fast_t c,
	uint8_t depth,
	uint8_t maxdepth,
	uint16_t *n,
	uint32_t *buf32
)
{
	uint8_t m, t, tinv, olddepth;
	uint32_t cc, oldvalue;
	uint64_t i;
	cube_fast_t d;

	oldvalue = buf32[coord_fast_cocsep(c)];
	if (depth == maxdepth) {
		if ((oldvalue & 0xFFU) != 0xFFU)
			return 0;

		for (t = 0, cc = 0; t < 48; t++) {
			d = transform(c, t);
			i = coord_fast_cocsep(d);
			tinv = inverse_trans(t);
			if ((buf32[i] & 0xFFU) == 0xFFU)
				cc++;
			buf32[i] = (*n << 16U) | (tinv << 8U) | depth;
		}
		(*n)++;

		return cc;
	}

	olddepth = (uint8_t)(oldvalue & 0xFFU);
	if (olddepth != depth)
		return 0;

	cc = 0;
	_foreach_move(m, c, d,
		cc += dfs_cocsep(d, depth+1, maxdepth, n, buf32);
	)

	return cc;
}
*/
