_static cube_t cubefromarray(uint8_t [static 8], uint8_t [static 12]);
_static cube_t solvedcube(void);
_static bool isconsistent(cube_t);
_static bool issolvable(cube_t);
_static bool issolved(cube_t);
_static bool iserror(cube_t);
_static void getcube_fix(int64_t *, int64_t *, int64_t *, int64_t *);
_static cube_t getcube(int64_t, int64_t, int64_t, int64_t);

_static cube_t
cubefromarray(uint8_t c[static 8], uint8_t e[static 12])
{
	return static_cube(
	    c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7],
	    e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7],
	    e[8], e[9], e[10], e[11]);
}

_static cube_t
solvedcube(void)
{
	return solved;
}

_static bool
isconsistent(cube_t cube)
{
	uint8_t i, p, e, piece, corner[8], edge[12];
	bool found[12];

	pieces(&cube, corner, edge);

	for (i = 0; i < 12; i++)
		found[i] = false;
	for (i = 0; i < 12; i++) {
		piece = edge[i];
		p = piece & _pbits;
		e = piece & _eobit;
		if (p >= 12)
			goto inconsistent_ep;
		if (e != 0 && e != _eobit)
			goto inconsistent_eo;
		found[p] = true;
	}
	for (i = 0; i < 12; i++)
		if (!found[i])
			goto inconsistent_ep;

	for (i = 0; i < 8; i++)
		found[i] = false;
	for (i = 0; i < 8; i++) {
		piece = corner[i];
		p = piece & _pbits;
		e = piece & _cobits;
		if (p >= 8)
			goto inconsistent_cp;
		if (e != 0 && e != _ctwist_cw && e != _ctwist_ccw)
			goto inconsistent_co;
		found[p] = true;
	}
	for (i = 0; i < 8; i++)
		if (!found[i])
			goto inconsistent_co;

	return true;

inconsistent_ep:
	LOG("Inconsistent EP\n");
	return false;
inconsistent_cp:
	LOG("Inconsistent CP\n");
	return false;
inconsistent_eo:
	LOG("Inconsistent EO\n");
	return false;
inconsistent_co:
	LOG("Inconsistent CO\n");
	return false;
}

_static bool
issolvable(cube_t cube)
{
	uint8_t i, eo, co, piece, edge[12], corner[8], ep[12], cp[8];

	DBG_ASSERT(isconsistent(cube), false,
	    "issolvable: cube is inconsistent\n");

	pieces(&cube, corner, edge);
	for (i = 0; i < 12; i++)
		ep[i] = edge[i] & _pbits;
	for (i = 0; i < 8; i++)
		cp[i] = corner[i] & _pbits;

	if (permsign(ep, 12) != permsign(cp, 8))
		goto issolvable_parity;

	eo = 0;
	for (i = 0; i < 12; i++) {
		piece = edge[i];
		eo += (piece & _eobit) >> _eoshift;
	}
	if (eo % 2 != 0)
		goto issolvable_eo;

	co = 0;
	for (i = 0; i < 8; i++) {
		piece = corner[i];
		co += (piece & _cobits) >> _coshift;
	}
	if (co % 3 != 0)
		goto issolvable_co;

	return true;

issolvable_parity:
	LOG("EP and CP parities are different\n");
	return false;
issolvable_eo:
	LOG("Odd number of flipped edges\n");
	return false;
issolvable_co:
	LOG("Sum of corner orientation is not multiple of 3\n");
	return false;
}

bool
issolved(cube_t cube)
{
	return equal(cube, solved);
}

bool
iserror(cube_t cube)
{
	return equal(cube, zero);
}

_static void
getcube_fix(int64_t *ep, int64_t *eo, int64_t *cp, int64_t *co)
{
	uint8_t e[12], c[8], coarr[8];

        *ep = (*ep % _12f + _12f) % _12f;
	*eo = (*eo % _2p11 + _2p11) % _2p11;
	*cp = (*cp % _8f + _8f) % _8f;
	*co = (*cp % _3p7 + _3p7) % _3p7;

	indextoperm(*ep, 12, e);
	indextoperm(*cp, 8, c);
	if (permsign(e, 12) != permsign(c, 8)) {
		_swap(c[0], c[1]);
		*cp = permtoindex(c, 8);

		sumzerotodigits(*co, 8, 3, coarr);
		_swap(coarr[0], coarr[1]);
		*co = digitstosumzero(coarr, 8, 3);
	}
}

_static cube_t
getcube(int64_t ep, int64_t eo, int64_t cp, int64_t co)
{
	uint8_t i, earr[12], carr[8], eoarr[12], coarr[8];

	sumzerotodigits(eo, 12, 2, eoarr);
	DBG_ASSERT(eoarr[0] != _error, zero, "Error making EO");
	indextoperm(ep, 12, earr);
	DBG_ASSERT(earr[0] != _error, zero, "Error making EP");
	for (i = 0; i < 12; i++)
		earr[i] |= eoarr[i] << _eoshift;

	sumzerotodigits(co, 8, 3, coarr);
	DBG_ASSERT(coarr[0] != _error, zero, "Error making CO");
	indextoperm(cp, 8, carr);
	DBG_ASSERT(carr[0] != _error, zero, "Error making CP");
	for (i = 0; i < 8; i++)
		carr[i] |= coarr[i] << _coshift;

	return cubefromarray(carr, earr);
}
