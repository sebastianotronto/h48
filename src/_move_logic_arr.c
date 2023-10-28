#define PERM4(r, i, j, k, l) \
    aux = r[i];              \
    r[i] = r[l];             \
    r[l] = r[k];             \
    r[k] = r[j];             \
    r[j] = aux;
#define PERM22(r, i, j, k, l) \
    aux = r[i];               \
    r[i] = r[j];              \
    r[j] = aux;               \
    aux = r[k];               \
    r[k] = r[l];              \
    r[l] = aux;
#define CO(a, b)                         \
    aux = (a & _cobits) + (b & _cobits); \
    auy = (aux + _ctwist_cw) >> 2U;      \
    auz = (aux + auy) & _cobits2;        \
    a = (a & _pbits) | auz;
#define CO4(r, i, j, k, l) \
    CO(r[i], _ctwist_cw)   \
    CO(r[j], _ctwist_cw)   \
    CO(r[k], _ctwist_ccw)  \
    CO(r[l], _ctwist_ccw)
#define EO4(r, i, j, k, l) \
    r[i] ^= _eobit;        \
    r[j] ^= _eobit;        \
    r[k] ^= _eobit;        \
    r[l] ^= _eobit;

uint8_t aux, auy, auz;
cube_t ret = c;

switch (m) {
case U:
	PERM4(ret.e, _e_uf, _e_ul, _e_ub, _e_ur)
	PERM4(ret.c, _c_ufr, _c_ufl, _c_ubl, _c_ubr)

	return ret;
case U2:
	PERM22(ret.e, _e_uf, _e_ub, _e_ul, _e_ur)
	PERM22(ret.c, _c_ufr, _c_ubl, _c_ufl, _c_ubr)

	return ret;
case U3:
	PERM4(ret.e, _e_uf, _e_ur, _e_ub, _e_ul)
	PERM4(ret.c, _c_ufr, _c_ubr, _c_ubl, _c_ufl)

	return ret;
case D:
	PERM4(ret.e, _e_df, _e_dr, _e_db, _e_dl)
	PERM4(ret.c, _c_dfr, _c_dbr, _c_dbl, _c_dfl)

	return ret;
case D2:
	PERM22(ret.e, _e_df, _e_db, _e_dr, _e_dl)
	PERM22(ret.c, _c_dfr, _c_dbl, _c_dbr, _c_dfl)

	return ret;
case D3:
	PERM4(ret.e, _e_df, _e_dl, _e_db, _e_dr)
	PERM4(ret.c, _c_dfr, _c_dfl, _c_dbl, _c_dbr)

	return ret;
case R:
	PERM4(ret.e, _e_ur, _e_br, _e_dr, _e_fr)
	PERM4(ret.c, _c_ufr, _c_ubr, _c_dbr, _c_dfr)

	CO4(ret.c, _c_ubr, _c_dfr, _c_ufr, _c_dbr)

	return ret;
case R2:
	PERM22(ret.e, _e_ur, _e_dr, _e_fr, _e_br)
	PERM22(ret.c, _c_ufr, _c_dbr, _c_ubr, _c_dfr)

	return ret;
case R3:
	PERM4(ret.e, _e_ur, _e_fr, _e_dr, _e_br)
	PERM4(ret.c, _c_ufr, _c_dfr, _c_dbr, _c_ubr)

	CO4(ret.c, _c_ubr, _c_dfr, _c_ufr, _c_dbr)

	return ret;
case L:
	PERM4(ret.e, _e_ul, _e_fl, _e_dl, _e_bl)
	PERM4(ret.c, _c_ufl, _c_dfl, _c_dbl, _c_ubl)

	CO4(ret.c, _c_ufl, _c_dbl, _c_dfl, _c_ubl)

	return ret;
case L2:
	PERM22(ret.e, _e_ul, _e_dl, _e_fl, _e_bl)
	PERM22(ret.c, _c_ufl, _c_dbl, _c_ubl, _c_dfl)

	return ret;
case L3:
	PERM4(ret.e, _e_ul, _e_bl, _e_dl, _e_fl)
	PERM4(ret.c, _c_ufl, _c_ubl, _c_dbl, _c_dfl)

	CO4(ret.c, _c_ufl, _c_dbl, _c_dfl, _c_ubl)

	return ret;
case F:
	PERM4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
	PERM4(ret.c, _c_ufr, _c_dfr, _c_dfl, _c_ufl)

	EO4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
	CO4(ret.c, _c_ufr, _c_dfl, _c_dfr, _c_ufl)

	return ret;
case F2:
	PERM22(ret.e, _e_uf, _e_df, _e_fr, _e_fl)
	PERM22(ret.c, _c_ufr, _c_dfl, _c_ufl, _c_dfr)

	return ret;
case F3:
	PERM4(ret.e, _e_uf, _e_fl, _e_df, _e_fr)
	PERM4(ret.c, _c_ufr, _c_ufl, _c_dfl, _c_dfr)

	EO4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
	CO4(ret.c, _c_ufr, _c_dfl, _c_dfr, _c_ufl)

	return ret;
case B:
	PERM4(ret.e, _e_ub, _e_bl, _e_db, _e_br)
	PERM4(ret.c, _c_ubr, _c_ubl, _c_dbl, _c_dbr)

	EO4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
	CO4(ret.c, _c_ubl, _c_dbr, _c_dbl, _c_ubr)

	return ret;
case B2:
	PERM22(ret.e, _e_ub, _e_db, _e_br, _e_bl)
	PERM22(ret.c, _c_ubr, _c_dbl, _c_ubl, _c_dbr)

	return ret;
case B3:
	PERM4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
	PERM4(ret.c, _c_ubr, _c_dbr, _c_dbl, _c_ubl)

	EO4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
	CO4(ret.c, _c_ubl, _c_dbr, _c_dbl, _c_ubr)

	return ret;
default:
#ifdef DEBUG
	fprintf(stderr, "mover error, unknown move\n");
#endif
	goto move_error;
}
