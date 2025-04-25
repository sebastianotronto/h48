#define DREOESEP_CLASSES UINT64_C(64430)
#define DREOESEP_MAX     (POW_2_11 * COMB_12_4)

STATIC uint64_t coord_dreoesep_nosym(cube_t);
STATIC cube_t invcoord_dreoesep_nosym(uint64_t);
STATIC cube_t coordinate_dr_merge(cube_t, cube_t);

STATIC uint64_t coordinate_dr_coord(cube_t, const unsigned char *);
STATIC cube_t coordinate_dr_cube(uint64_t, const unsigned char *);
STATIC bool coordinate_dr_isnasty(uint64_t, const unsigned char *);
STATIC size_t coordinate_dr_gendata(unsigned char *);

STATIC bool is_eoco_solvable(cube_t);

/* TODO: remove the following two when all coordinates are converted to unsigned */
STATIC uint64_t coord_co_u(cube_t c) { return (uint64_t)coord_co(c); }
STATIC cube_t invcoord_co_u(uint64_t i) { return invcoord_co((int64_t)i); }

STATIC coord_t coordinate_dr = {
	.name = "DR",
	.coord = &coordinate_dr_coord,
	.cube = &coordinate_dr_cube,
	.isnasty = &coordinate_dr_isnasty,
	.gendata = coordinate_dr_gendata,
	.max = DREOESEP_CLASSES * POW_3_7,
	.trans_mask = TM_UDFIX,
	.moves_mask = MM18_ALLMOVES,
	.axistrans = {
		[AXIS_UD] = TRANS_UFr,
		[AXIS_RL] = TRANS_RFr,
		[AXIS_FB] = TRANS_FDr,
	},
	.is_admissible = &solution_lastqt_cw,
	.is_solvable = &is_eoco_solvable,
	.sym = {
		.classes = DREOESEP_CLASSES,
		.max = DREOESEP_MAX,
		.coord = &coord_dreoesep_nosym,
		.cube = &invcoord_dreoesep_nosym,
		.max2 = POW_3_7,
		.coord2 = &coord_co_u,
		.cube2 = &invcoord_co_u,
		.merge = &coordinate_dr_merge,
	},
};

STATIC uint64_t
coord_dreoesep_nosym(cube_t cube)
{
	uint64_t eo, esep;

	esep = coord_esep(cube) / COMB_8_4;
	eo = coord_eo(cube);

	return esep * POW_2_11 + eo;
}

STATIC cube_t
invcoord_dreoesep_nosym(uint64_t coord)
{
	uint64_t eo, esep;
	cube_t cube;

	eo = coord % POW_2_11;
	esep = (coord / POW_2_11) * COMB_8_4; 
	cube = invcoord_esep(esep);
	set_eo(&cube, eo);

	return cube;
}

STATIC cube_t
coordinate_dr_merge(cube_t c1, cube_t c2)
{
	cube_t merged;

	merged = c1;
	copy_corners(&merged, c2);

	return merged;
}

STATIC uint64_t
coordinate_dr_coord(cube_t cube, const unsigned char *data)
{
	return coord_coord_generic(&coordinate_dr, cube, data);
}

STATIC cube_t
coordinate_dr_cube(uint64_t i, const unsigned char *data)
{
	return coord_cube_generic(&coordinate_dr, i, data);
}

STATIC bool
coordinate_dr_isnasty(uint64_t i, const unsigned char *data)
{
	return coord_isnasty_generic(&coordinate_dr, i, data);
}

STATIC size_t
coordinate_dr_gendata(unsigned char *data)
{
	return coord_gendata_generic(&coordinate_dr, data);
}

STATIC bool
is_eoco_solvable(cube_t cube) {
	uint8_t c[8], e[12], i, cocount, eocount;

	pieces(&cube, c, e);

	for (i = 0, cocount = 0; i < 8; i++)
		cocount += (c[i] & COBITS_2) >> COSHIFT;

	for (i = 0, eocount = 0; i < 12; i++)
		eocount += (e[i] & EOBIT) >> EOSHIFT;

	return cocount % 3 == 0 && eocount % 2 == 0;
}
