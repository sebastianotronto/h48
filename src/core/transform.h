STATIC cube_t transform_edges(cube_t, uint8_t);
STATIC cube_t transform_corners(cube_t, uint8_t);
STATIC cube_t transform(cube_t, uint8_t);
STATIC cube_t applytrans(cube_t, const char *);

static uint8_t cube_trans_table[48][20] = {
	[TRANS_UFr] = { TRANS_CUBE_UFr },
	[TRANS_UFm] = { TRANS_CUBE_UFm },
	[TRANS_ULr] = { TRANS_CUBE_ULr },
	[TRANS_ULm] = { TRANS_CUBE_ULm },
	[TRANS_UBr] = { TRANS_CUBE_UBr },
	[TRANS_UBm] = { TRANS_CUBE_UBm },
	[TRANS_URr] = { TRANS_CUBE_URr },
	[TRANS_URm] = { TRANS_CUBE_URm },
	[TRANS_DFr] = { TRANS_CUBE_DFr },
	[TRANS_DFm] = { TRANS_CUBE_DFm },
	[TRANS_DLr] = { TRANS_CUBE_DLr },
	[TRANS_DLm] = { TRANS_CUBE_DLm },
	[TRANS_DBr] = { TRANS_CUBE_DBr },
	[TRANS_DBm] = { TRANS_CUBE_DBm },
	[TRANS_DRr] = { TRANS_CUBE_DRr },
	[TRANS_DRm] = { TRANS_CUBE_DRm },
	[TRANS_RUr] = { TRANS_CUBE_RUr },
	[TRANS_RUm] = { TRANS_CUBE_RUm },
	[TRANS_RFr] = { TRANS_CUBE_RFr },
	[TRANS_RFm] = { TRANS_CUBE_RFm },
	[TRANS_RDr] = { TRANS_CUBE_RDr },
	[TRANS_RDm] = { TRANS_CUBE_RDm },
	[TRANS_RBr] = { TRANS_CUBE_RBr },
	[TRANS_RBm] = { TRANS_CUBE_RBm },
	[TRANS_LUr] = { TRANS_CUBE_LUr },
	[TRANS_LUm] = { TRANS_CUBE_LUm },
	[TRANS_LFr] = { TRANS_CUBE_LFr },
	[TRANS_LFm] = { TRANS_CUBE_LFm },
	[TRANS_LDr] = { TRANS_CUBE_LDr },
	[TRANS_LDm] = { TRANS_CUBE_LDm },
	[TRANS_LBr] = { TRANS_CUBE_LBr },
	[TRANS_LBm] = { TRANS_CUBE_LBm },
	[TRANS_FUr] = { TRANS_CUBE_FUr },
	[TRANS_FUm] = { TRANS_CUBE_FUm },
	[TRANS_FRr] = { TRANS_CUBE_FRr },
	[TRANS_FRm] = { TRANS_CUBE_FRm },
	[TRANS_FDr] = { TRANS_CUBE_FDr },
	[TRANS_FDm] = { TRANS_CUBE_FDm },
	[TRANS_FLr] = { TRANS_CUBE_FLr },
	[TRANS_FLm] = { TRANS_CUBE_FLm },
	[TRANS_BUr] = { TRANS_CUBE_BUr },
	[TRANS_BUm] = { TRANS_CUBE_BUm },
	[TRANS_BRr] = { TRANS_CUBE_BRr },
	[TRANS_BRm] = { TRANS_CUBE_BRm },
	[TRANS_BDr] = { TRANS_CUBE_BDr },
	[TRANS_BDm] = { TRANS_CUBE_BDm },
	[TRANS_BLr] = { TRANS_CUBE_BLr },
	[TRANS_BLm] = { TRANS_CUBE_BLm },
};

static uint8_t cube_trans_table_inverse[48][20] = {
	[TRANS_UFr] = { TRANS_CUBE_UFr_INVERSE },
	[TRANS_UFm] = { TRANS_CUBE_UFm_INVERSE },
	[TRANS_ULr] = { TRANS_CUBE_ULr_INVERSE },
	[TRANS_ULm] = { TRANS_CUBE_ULm_INVERSE },
	[TRANS_UBr] = { TRANS_CUBE_UBr_INVERSE },
	[TRANS_UBm] = { TRANS_CUBE_UBm_INVERSE },
	[TRANS_URr] = { TRANS_CUBE_URr_INVERSE },
	[TRANS_URm] = { TRANS_CUBE_URm_INVERSE },
	[TRANS_DFr] = { TRANS_CUBE_DFr_INVERSE },
	[TRANS_DFm] = { TRANS_CUBE_DFm_INVERSE },
	[TRANS_DLr] = { TRANS_CUBE_DLr_INVERSE },
	[TRANS_DLm] = { TRANS_CUBE_DLm_INVERSE },
	[TRANS_DBr] = { TRANS_CUBE_DBr_INVERSE },
	[TRANS_DBm] = { TRANS_CUBE_DBm_INVERSE },
	[TRANS_DRr] = { TRANS_CUBE_DRr_INVERSE },
	[TRANS_DRm] = { TRANS_CUBE_DRm_INVERSE },
	[TRANS_RUr] = { TRANS_CUBE_RUr_INVERSE },
	[TRANS_RUm] = { TRANS_CUBE_RUm_INVERSE },
	[TRANS_RFr] = { TRANS_CUBE_RFr_INVERSE },
	[TRANS_RFm] = { TRANS_CUBE_RFm_INVERSE },
	[TRANS_RDr] = { TRANS_CUBE_RDr_INVERSE },
	[TRANS_RDm] = { TRANS_CUBE_RDm_INVERSE },
	[TRANS_RBr] = { TRANS_CUBE_RBr_INVERSE },
	[TRANS_RBm] = { TRANS_CUBE_RBm_INVERSE },
	[TRANS_LUr] = { TRANS_CUBE_LUr_INVERSE },
	[TRANS_LUm] = { TRANS_CUBE_LUm_INVERSE },
	[TRANS_LFr] = { TRANS_CUBE_LFr_INVERSE },
	[TRANS_LFm] = { TRANS_CUBE_LFm_INVERSE },
	[TRANS_LDr] = { TRANS_CUBE_LDr_INVERSE },
	[TRANS_LDm] = { TRANS_CUBE_LDm_INVERSE },
	[TRANS_LBr] = { TRANS_CUBE_LBr_INVERSE },
	[TRANS_LBm] = { TRANS_CUBE_LBm_INVERSE },
	[TRANS_FUr] = { TRANS_CUBE_FUr_INVERSE },
	[TRANS_FUm] = { TRANS_CUBE_FUm_INVERSE },
	[TRANS_FRr] = { TRANS_CUBE_FRr_INVERSE },
	[TRANS_FRm] = { TRANS_CUBE_FRm_INVERSE },
	[TRANS_FDr] = { TRANS_CUBE_FDr_INVERSE },
	[TRANS_FDm] = { TRANS_CUBE_FDm_INVERSE },
	[TRANS_FLr] = { TRANS_CUBE_FLr_INVERSE },
	[TRANS_FLm] = { TRANS_CUBE_FLm_INVERSE },
	[TRANS_BUr] = { TRANS_CUBE_BUr_INVERSE },
	[TRANS_BUm] = { TRANS_CUBE_BUm_INVERSE },
	[TRANS_BRr] = { TRANS_CUBE_BRr_INVERSE },
	[TRANS_BRm] = { TRANS_CUBE_BRm_INVERSE },
	[TRANS_BDr] = { TRANS_CUBE_BDr_INVERSE },
	[TRANS_BDm] = { TRANS_CUBE_BDm_INVERSE },
	[TRANS_BLr] = { TRANS_CUBE_BLr_INVERSE },
	[TRANS_BLm] = { TRANS_CUBE_BLm_INVERSE },
};

STATIC cube_t
transform_edges(cube_t c, uint8_t t)
{
	cube_t ret, trans_cube, trans_inv;

	DBG_ASSERT(t < 48, ZERO_CUBE,
	    "transform: invalid transformation %" PRIu8
	    ", must be between 0 and 47\n", t);

	trans_cube = cubefromarray_single(cube_trans_table[t]);
	trans_inv = cubefromarray_single(cube_trans_table_inverse[t]);

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

	trans_cube = cubefromarray_single(cube_trans_table[t]);
	trans_inv = cubefromarray_single(cube_trans_table_inverse[t]);

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

	trans_cube = cubefromarray_single(cube_trans_table[t]);
	trans_inv = cubefromarray_single(cube_trans_table_inverse[t]);

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
