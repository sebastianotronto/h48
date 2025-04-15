STATIC void solution_moves_reset(solution_moves_t [static 1]);
STATIC void solution_moves_transform(solution_moves_t [static 1], uint8_t t);
STATIC bool solution_list_init(
    solution_list_t [static 1], size_t n, char [n]);
STATIC bool solution_moves_equal(
    const solution_moves_t [static 1], const solution_moves_t [static 1]);
STATIC bool solution_moves_is_duplicate(size_t n, const solution_moves_t[n+1]);
STATIC bool appendchar(solution_list_t [static 1], char);
STATIC int64_t appendsolution(const solution_moves_t [static 1],
    const solution_settings_t [static 1], solution_list_t [static 1], bool);
STATIC bool solutions_done(const solution_list_t [static 1],
    const solution_settings_t [static 1], int8_t depth);

STATIC void
solution_moves_reset(solution_moves_t sol[static 1])
{
	sol->nmoves = 0;
	sol->npremoves = 0;
}

STATIC void
solution_moves_transform(solution_moves_t moves[static 1], uint8_t t)
{
	uint8_t i;

	for (i = 0; i < moves->nmoves; i++)
		moves->moves[i] = transform_move(moves->moves[i], t);

	for (i = 0; i < moves->npremoves; i++)
		moves->premoves[i] = transform_move(moves->premoves[i], t);
}

STATIC bool
solution_list_init(solution_list_t sols[static 1], size_t n, char buf[n])
{
	if (n == 0) {
		LOG("Cannot use solution buffer with size 0\n");
		return false;
	}

	sols->nsols = 0;
	sols->shortest_sol = MAXLEN + 1;
	sols->size = n;
	sols->used = 0;
	sols->buf = buf;

	/* Ensure string buffer is NULL-terminated */
	sols->buf[0] = '\0';

	return true;
}

STATIC bool
solution_moves_equal(
	const solution_moves_t a[static 1],
	const solution_moves_t b[static 1]
)
{
	uint8_t i;

	if (a->nmoves != b->nmoves || a->npremoves != b->npremoves)
		return false;

	for (i = 0; i < a->nmoves; i++)
		if (a->moves[i] != b->moves[i])
			return false;

	for (i = 0; i < a->npremoves; i++)
		if (a->premoves[i] != b->premoves[i])
			return false;

	return true;
}

STATIC bool
solution_moves_is_duplicate(size_t n, const solution_moves_t s[n+1])
{
	size_t i;

	for (i = 0; i < n; i++)
		if (solution_moves_equal(&s[i], &s[n]))
			return true;

	return false;
}

STATIC bool
appendchar(solution_list_t solutions[static 1], char c)
{
	if (solutions->size <= solutions->used)
		return false;

	solutions->buf[solutions->used++] = c;

	return true;
}

STATIC int64_t
appendsolution(
	const solution_moves_t moves[static 1],
	const solution_settings_t settings[static 1],
	solution_list_t list[static 1],
	bool log
)
{
	int64_t r, strl;
	int i;
	uint8_t t;
	solution_moves_t tsol[NTRANS];
	char *last_start;

	if (moves->nmoves + moves->npremoves > MAXLEN)
		goto appendsolution_error_solution_length;

	for (
	    t = 0, r = 0;
	    t < NTRANS && list->nsols < settings->maxsolutions;
	    t++
	) {
		if (!(settings->tmask & TM_SINGLE(t)))
			continue;

		tsol[r] = *moves;
		if (settings->unniss) {
			tsol[r].nmoves += moves->npremoves;
			tsol[r].npremoves = 0;
			for (i = moves->npremoves-1; i >= 0; i--)
				tsol[r].moves[tsol[r].nmoves - i - 1] =
				    inverse_move(moves->premoves[i]);

			/*
			This is a bit ugly: we have to sort now and then again
			later, because the allowedmoves check would fail with
			improperly sorted parallel moves, but then transforming
			could swap the pairs the wrong way around.
			TODO: maybe fix this
			*/
			sortparallel_moves(tsol[r].nmoves, tsol[r].moves);

			/* Check if unnissed premoves cancel with normal. */
			if (!allowedmoves(tsol[r].nmoves, tsol[r].moves))
				continue;
		}
		solution_moves_transform(&tsol[r], t);
		sortparallel_moves(tsol[r].nmoves, tsol[r].moves);
		sortparallel_moves(tsol[r].npremoves, tsol[r].premoves);

		/* Skip duplicates that may appear after transforming */
		if (solution_moves_is_duplicate(r, tsol))
			continue;

		last_start = list->buf + list->used;

		/* Write moves on normal */
		strl = writemoves(tsol[r].nmoves, tsol[r].moves,
		    list->size - list->used, list->buf + list->used);
		if (strl < 0)
			goto appendsolution_error_buffer;
		list->used += strl;

		/* Write moves on inverse with NISS notation */
		if (tsol[r].npremoves > 0) {
			if (strl > 0)
				if (!appendchar(list, ' '))
					goto appendsolution_error_buffer;
			if (!appendchar(list, '('))
				goto appendsolution_error_buffer;

			strl = writemoves(tsol[r].npremoves, tsol[r].premoves,
			    list->size - list->used, list->buf + list->used);
			if (strl < 0)
				goto appendsolution_error_buffer;
			list->used += strl;

			if (!appendchar(list, ')'))
				goto appendsolution_error_buffer;
		}

		if (!appendchar(list, '\n'))
			goto appendsolution_error_buffer;

		++list->nsols;
		list->shortest_sol = MIN(
		    list->shortest_sol, tsol[r].nmoves + tsol[r].npremoves);
		r++;

		if (log) {
			list->buf[list->used-1] = '\0';
			LOG("Found solution #%" PRIu64 ": %s\n",
			    list->nsols, last_start);
			list->buf[list->used-1] = '\n';
		}
	}

	list->buf[list->used] = '\0';
	return r;

appendsolution_error_buffer:
	LOG("Could not append solution to buffer: size too small\n");
	list->buf[0] = '\0';
	return NISSY_ERROR_BUFFER_SIZE;

appendsolution_error_solution_length:
	LOG("Error: solution is too long (%" PRIu8 ").\n"
	    "This is a bug, please report it.\n",
	    moves->nmoves + moves->npremoves);
	list->buf[0] = '\0';
	return NISSY_ERROR_UNKNOWN;
}

STATIC bool
solutions_done(
	const solution_list_t list[static 1],
	const solution_settings_t settings[static 1],
	int8_t depth
)
{
	if (list->nsols >= settings->maxsolutions)
		return true;

	if (depth > settings->maxmoves)
		return true;

	if (list->nsols > 0 && settings->optimal >= 0 &&
	    depth > list->shortest_sol + settings->optimal)
		return true;

	return false;
}
