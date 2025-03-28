#define DREOESEP_CLASSES   UINT64_C(64430)
#define DREOESEP_MAX       (POW_2_11 * COMB_12_4)

#define DR_CLASS_TABLESIZE (sizeof(uint32_t) * (size_t)DREOESEP_MAX)
#define DR_REP_TABLESIZE   (sizeof(uint32_t) * (size_t)DREOESEP_CLASSES)
#define DR_COORD_DATASIZE  (INFOSIZE + DR_CLASS_TABLESIZE + DR_REP_TABLESIZE)

#define DR_CLASS_SHIFT     UINT32_C(16)
#define DR_CLASS_MASK      (UINT32_C(0xFFFF) << DR_CLASS_SHIFT)
#define DR_CLASS(d)        (((d) & DR_CLASS_MASK) >> DR_CLASS_SHIFT)

#define DR_TTREP_SHIFT     UINT32_C(0)
#define DR_TTREP_MASK      (UINT32_C(0xFF) << DR_TTREP_SHIFT)
#define DR_TTREP(d)        (((d) & DR_TTREP_MASK) >> DR_TTREP_SHIFT)

#define DR_ISNASTY_SHIFT   UINT32_C(8)
#define DR_ISNASTY_MASK    (UINT32_C(0xFF) << DR_ISNASTY_SHIFT)
#define DR_ISNASTY(d)      (((d) & DR_ISNASTY_MASK) >> DR_ISNASTY_SHIFT)

STATIC uint64_t coord_dreoesep_nosym(cube_t);
STATIC cube_t invcoord_dreoesep_nosym(uint64_t);

STATIC uint64_t coordinate_dr_coord(cube_t, const void *);
STATIC cube_t coordinate_dr_cube(uint64_t, const void *);
STATIC bool coordinate_dr_isnasty(uint64_t, const void *);
STATIC uint64_t coordinate_dr_gendata(void *);

STATIC coord_t coordinate_dr = {
	.name = "DR",
	.coord = &coordinate_dr_coord,
	.cube = &coordinate_dr_cube,
	.isnasty = &coordinate_dr_isnasty,
	.gendata = coordinate_dr_gendata,
	.max = DREOESEP_CLASSES * POW_3_7,
	.trans_mask = TM_UDFIX,
	.moves_mask = MM_ALLMOVES,
	.axistrans = {
		[AXIS_UD] = TRANS_UFr,
		[AXIS_RL] = TRANS_RFr,
		[AXIS_FB] = TRANS_FDr,
	},
	.is_admissible = &solution_lastqt_cw,
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

STATIC uint64_t
coordinate_dr_coord(cube_t cube, const void *data)
{
	const char *datanoinfo;
	const uint32_t *data32;
	uint32_t d;
	cube_t transformed;

	datanoinfo = (const char *)data + INFOSIZE;
	data32 = (const uint32_t *)datanoinfo;
	d = data32[coord_dreoesep_nosym(cube)];
	transformed = transform(cube, DR_TTREP(d));
	
	return DR_CLASS(d) * POW_3_7 + coord_co(transformed);
}

STATIC cube_t
coordinate_dr_cube(uint64_t coord, const void *data)
{
	const char *datanoinfo;
	const uint32_t *rep32;
	cube_t cube;

	datanoinfo = (const char *)data + INFOSIZE;
	rep32 = (const uint32_t *)(datanoinfo + DR_CLASS_TABLESIZE);
	cube = invcoord_dreoesep_nosym(rep32[coord / POW_3_7]);
	copy_corners(&cube, invcoord_co(coord % POW_3_7));

	return cube;
}

STATIC bool
coordinate_dr_isnasty(uint64_t coord, const void *data)
{
	const char *datanoinfo;
	const uint32_t *classttrep;

	datanoinfo = (const char *)data + INFOSIZE;
	classttrep = (const uint32_t *)datanoinfo;

	return DR_ISNASTY(classttrep[coord]);
}

STATIC size_t
coordinate_dr_gendata(void *data)
{
	uint64_t i, ii, j, n, t, nasty;
	char *datanoinfo;
	uint32_t *classttrep, *rep;
	cube_t c;
	tableinfo_t info;

	if (data == NULL)
		goto coordinate_dr_gendata_returnsize;

	datanoinfo = (char *)data + INFOSIZE;
	classttrep = (uint32_t *)datanoinfo;
	rep = classttrep + (DR_CLASS_TABLESIZE / sizeof(uint32_t));
	memset(data, 0xFF, DR_COORD_DATASIZE);

	info = (tableinfo_t) {
		.solver = "coord data for DR",
		.type = TABLETYPE_SPECIAL,
		.infosize = INFOSIZE,
		.fullsize = DR_COORD_DATASIZE,
		.hash = 0,
		.entries = DREOESEP_CLASSES + DREOESEP_MAX,
		.classes = DREOESEP_CLASSES,
		.bits = 32,
		.base = 0,
		.maxvalue = 0,
		.next = 0
	};

	for (i = 0, n = 0; i < COMB_12_4 * POW_2_11; i++) {
		if (classttrep[i] != 0xFFFFFFFF)
			continue;

		c = invcoord_dreoesep_nosym(i);
		ii = coord_dreoesep_nosym(c);
		for (t = 0, nasty = 0; t < NTRANS && !nasty; t++) {
			if (!((UINT64_C(1) << t) & coordinate_dr.trans_mask))
				continue;

			nasty = ii == coord_dreoesep_nosym(transform(c, t));
		}

		for (t = 0; t < NTRANS; t++) {
			if (!((UINT64_C(1) << t) & coordinate_dr.trans_mask))
				continue;

			j = coord_dreoesep_nosym(transform(c, t));
			classttrep[j] =
			    (n << DR_CLASS_SHIFT) |
			    (nasty << DR_ISNASTY_SHIFT) |
			    (inverse_trans(t) << DR_TTREP_SHIFT);
		}
		rep[n++] = i;
	}

	writetableinfo(&info, DR_COORD_DATASIZE, data);

	DBG_ASSERT(n == DREOESEP_CLASSES, 0,
	    "dr coordinate data: computed %" PRIu64 " classes, "
	    "expected %" PRIu64 "\n", n, DREOESEP_CLASSES);

coordinate_dr_gendata_returnsize:
	return DR_COORD_DATASIZE;
}
