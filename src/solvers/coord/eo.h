STATIC uint64_t coordinate_eo_coord(cube_t, const unsigned char *);
STATIC cube_t coordinate_eo_cube(uint64_t, const unsigned char *);
STATIC bool coordinate_eo_isnasty(uint64_t, const unsigned char *);
STATIC size_t coordinate_eo_gendata(unsigned char *);
STATIC bool is_eo_even(cube_t);

STATIC coord_t coordinate_eo = {
	.name = "EO",
	.coord = &coordinate_eo_coord,
	.cube = &coordinate_eo_cube,
	.isnasty = &coordinate_eo_isnasty,
	.gendata = coordinate_eo_gendata,
	.max = POW_2_11,
	.trans_mask = TM_SINGLE(TRANS_UFr),
	.moves_mask = MM_ALLMOVES,
	.axistrans = {
		[AXIS_UD] = TRANS_FDr,
		[AXIS_RL] = TRANS_URr,
		[AXIS_FB] = TRANS_UFr,
	},
	.is_admissible = &solution_lastqt_cw,
	.is_solvable = &is_eo_even,
	.sym = {0},
};

STATIC uint64_t
coordinate_eo_coord(cube_t c, const unsigned char *data)
{
	return (uint64_t)coord_eo(c);
}

STATIC cube_t
coordinate_eo_cube(uint64_t c, const unsigned char *data)
{
	cube_t cube = SOLVED_CUBE;
	set_eo(&cube, (int64_t)c);
	return cube;
}

STATIC bool
coordinate_eo_isnasty(uint64_t c, const unsigned char *data)
{
	return false;
}

STATIC size_t
coordinate_eo_gendata(unsigned char *data)
{
	return 0;
}

STATIC bool
is_eo_even(cube_t cube)
{
	uint8_t c[8], e[12], i, count;

	pieces(&cube, c, e);

	for (i = 0, count = 0; i < 12; i++)
		count += (e[i] & EOBIT) >> EOSHIFT;

	return count % 2 == 0;
}
