STATIC cube_t transform_edges(cube_t, uint8_t);
STATIC cube_t transform_corners(cube_t, uint8_t);
STATIC cube_t transform(cube_t, uint8_t);
STATIC cube_t applytrans(cube_t, const char *);

static cube_t cube_trans_table[48] = {
	[TRANS_UFr] = TRANS_CUBE_UFr,
	[TRANS_UFm] = TRANS_CUBE_UFm,
	[TRANS_ULr] = TRANS_CUBE_URr,
	[TRANS_ULm] = TRANS_CUBE_ULm,
	[TRANS_UBr] = TRANS_CUBE_UBr,
	[TRANS_UBm] = TRANS_CUBE_UBm,
	[TRANS_URr] = TRANS_CUBE_ULr,
	[TRANS_URm] = TRANS_CUBE_URm,
	[TRANS_DFr] = TRANS_CUBE_DFr,
	[TRANS_DFm] = TRANS_CUBE_DFm,
	[TRANS_DLr] = TRANS_CUBE_DLr,
	[TRANS_DLm] = TRANS_CUBE_DRm,
	[TRANS_DBr] = TRANS_CUBE_DBr,
	[TRANS_DBm] = TRANS_CUBE_DBm,
	[TRANS_DRr] = TRANS_CUBE_DRr,
	[TRANS_DRm] = TRANS_CUBE_DLm,
	[TRANS_RUr] = TRANS_CUBE_FRr,
	[TRANS_RUm] = TRANS_CUBE_FLm,
	[TRANS_RFr] = TRANS_CUBE_LFr,
	[TRANS_RFm] = TRANS_CUBE_RFm,
	[TRANS_RDr] = TRANS_CUBE_BLr,
	[TRANS_RDm] = TRANS_CUBE_BRm,
	[TRANS_RBr] = TRANS_CUBE_RBr,
	[TRANS_RBm] = TRANS_CUBE_LBm,
	[TRANS_LUr] = TRANS_CUBE_FLr,
	[TRANS_LUm] = TRANS_CUBE_FRm,
	[TRANS_LFr] = TRANS_CUBE_RFr,
	[TRANS_LFm] = TRANS_CUBE_LFm,
	[TRANS_LDr] = TRANS_CUBE_BRr,
	[TRANS_LDm] = TRANS_CUBE_BLm,
	[TRANS_LBr] = TRANS_CUBE_LBr,
	[TRANS_LBm] = TRANS_CUBE_RBm,
	[TRANS_FUr] = TRANS_CUBE_FUr,
	[TRANS_FUm] = TRANS_CUBE_FUm,
	[TRANS_FRr] = TRANS_CUBE_RUr,
	[TRANS_FRm] = TRANS_CUBE_LUm,
	[TRANS_FDr] = TRANS_CUBE_BUr,
	[TRANS_FDm] = TRANS_CUBE_BUm,
	[TRANS_FLr] = TRANS_CUBE_LUr,
	[TRANS_FLm] = TRANS_CUBE_RUm,
	[TRANS_BUr] = TRANS_CUBE_FDr,
	[TRANS_BUm] = TRANS_CUBE_FDm,
	[TRANS_BRr] = TRANS_CUBE_LDr,
	[TRANS_BRm] = TRANS_CUBE_RDm,
	[TRANS_BDr] = TRANS_CUBE_BDr,
	[TRANS_BDm] = TRANS_CUBE_BDm,
	[TRANS_BLr] = TRANS_CUBE_RDr,
	[TRANS_BLm] = TRANS_CUBE_LDm,
};

static cube_t cube_trans_table_inverse[48] = {
	[TRANS_UFr] = TRANS_CUBE_UFr_inverse,
	[TRANS_UFm] = TRANS_CUBE_UFm_inverse,
	[TRANS_ULr] = TRANS_CUBE_URr_inverse,
	[TRANS_ULm] = TRANS_CUBE_ULm_inverse,
	[TRANS_UBr] = TRANS_CUBE_UBr_inverse,
	[TRANS_UBm] = TRANS_CUBE_UBm_inverse,
	[TRANS_URr] = TRANS_CUBE_ULr_inverse,
	[TRANS_URm] = TRANS_CUBE_URm_inverse,
	[TRANS_DFr] = TRANS_CUBE_DFr_inverse,
	[TRANS_DFm] = TRANS_CUBE_DFm_inverse,
	[TRANS_DLr] = TRANS_CUBE_DLr_inverse,
	[TRANS_DLm] = TRANS_CUBE_DRm_inverse,
	[TRANS_DBr] = TRANS_CUBE_DBr_inverse,
	[TRANS_DBm] = TRANS_CUBE_DBm_inverse,
	[TRANS_DRr] = TRANS_CUBE_DRr_inverse,
	[TRANS_DRm] = TRANS_CUBE_DLm_inverse,
	[TRANS_RUr] = TRANS_CUBE_FRr_inverse,
	[TRANS_RUm] = TRANS_CUBE_FLm_inverse,
	[TRANS_RFr] = TRANS_CUBE_LFr_inverse,
	[TRANS_RFm] = TRANS_CUBE_RFm_inverse,
	[TRANS_RDr] = TRANS_CUBE_BLr_inverse,
	[TRANS_RDm] = TRANS_CUBE_BRm_inverse,
	[TRANS_RBr] = TRANS_CUBE_RBr_inverse,
	[TRANS_RBm] = TRANS_CUBE_LBm_inverse,
	[TRANS_LUr] = TRANS_CUBE_FLr_inverse,
	[TRANS_LUm] = TRANS_CUBE_FRm_inverse,
	[TRANS_LFr] = TRANS_CUBE_RFr_inverse,
	[TRANS_LFm] = TRANS_CUBE_LFm_inverse,
	[TRANS_LDr] = TRANS_CUBE_BRr_inverse,
	[TRANS_LDm] = TRANS_CUBE_BLm_inverse,
	[TRANS_LBr] = TRANS_CUBE_LBr_inverse,
	[TRANS_LBm] = TRANS_CUBE_RBm_inverse,
	[TRANS_FUr] = TRANS_CUBE_FUr_inverse,
	[TRANS_FUm] = TRANS_CUBE_FUm_inverse,
	[TRANS_FRr] = TRANS_CUBE_RUr_inverse,
	[TRANS_FRm] = TRANS_CUBE_LUm_inverse,
	[TRANS_FDr] = TRANS_CUBE_BUr_inverse,
	[TRANS_FDm] = TRANS_CUBE_BUm_inverse,
	[TRANS_FLr] = TRANS_CUBE_LUr_inverse,
	[TRANS_FLm] = TRANS_CUBE_RUm_inverse,
	[TRANS_BUr] = TRANS_CUBE_FDr_inverse,
	[TRANS_BUm] = TRANS_CUBE_FDm_inverse,
	[TRANS_BRr] = TRANS_CUBE_LDr_inverse,
	[TRANS_BRm] = TRANS_CUBE_RDm_inverse,
	[TRANS_BDr] = TRANS_CUBE_BDr_inverse,
	[TRANS_BDm] = TRANS_CUBE_BDm_inverse,
	[TRANS_BLr] = TRANS_CUBE_RDr_inverse,
	[TRANS_BLm] = TRANS_CUBE_LDm_inverse,
};

STATIC cube_t
transform_edges(cube_t c, uint8_t t)
{
	cube_t ret, trans_cube, trans_inv;

	DBG_ASSERT(t < 48, ZERO_CUBE,
	    "transform: invalid transformation %" PRIu8
	    ", must be between 0 and 47\n", t);

	trans_cube = cube_trans_table[t];
	trans_inv = cube_trans_table_inverse[t];

	ret = compose_edges(trans_cube, c);
	ret = compose_edges(ret, trans_inv);

	return ret;
}

STATIC cube_t
transform_corners(cube_t c, uint8_t t)
{
	cube_t ret, trans_cube, trans_inv;

	DBG_ASSERT(t < 48, ZERO_CUBE,
	    "transform: invalid transformation %" PRIu8
	    ", must be between 0 and 47\n", t);

	trans_cube = cube_trans_table[t];
	trans_inv = cube_trans_table_inverse[t];

	ret = compose_corners(trans_cube, c);
	ret = compose_corners(ret, trans_inv);

	return t < 24 ? ret : invertco(ret);
}

STATIC cube_t
transform(cube_t c, uint8_t t)
{
	cube_t ret, trans_cube, trans_inv;

	DBG_ASSERT(t < 48, ZERO_CUBE,
	    "transform: invalid transformation %" PRIu8
	    ", must be between 0 and 47\n", t);

	trans_cube = cube_trans_table[t];
	trans_inv = cube_trans_table_inverse[t];

	ret = compose(trans_cube, c);
	ret = compose(ret, trans_inv);

	return t < 24 ? ret : invertco(ret);
}

STATIC cube_t
applytrans(cube_t cube, const char *buf)
{
	uint8_t t;

	DBG_ASSERT(isconsistent(cube), ZERO_CUBE,
	    "transformation error: inconsistent cube\n");

	t = readtrans(buf);

	return transform(cube, t);
}
