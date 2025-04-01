STATIC uint64_t coordinate_eo_coord(cube_t, const void *);
STATIC cube_t coordinate_eo_cube(uint64_t, const void *);
STATIC bool coordinate_eo_isnasty(uint64_t, const void *);
STATIC size_t coordinate_eo_gendata(void *);

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
};

STATIC uint64_t
coordinate_eo_coord(cube_t c, const void *data)
{
	return (uint64_t)coord_eo(c);
}

STATIC cube_t
coordinate_eo_cube(uint64_t c, const void *data)
{
	cube_t cube = SOLVED_CUBE;
	set_eo(&cube, (int64_t)c);
	return cube;
}

STATIC bool
coordinate_eo_isnasty(uint64_t c, const void *data)
{
	return false;
}

STATIC size_t
coordinate_eo_gendata(void *data)
{
	return 0;
}
