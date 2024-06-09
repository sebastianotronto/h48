/* TODO: remove these macros, if the below works */

#define _trans_edges_rotation(T, c) \
        compose_edges(compose_edges(_trans_cube_ ## T, c), \
        _trans_cube_ ## T ## _inverse)
#define _trans_edges_mirrored(T, c) _trans_edges_rotation(T, c)

#define _trans_corners_rotation(T, c) \
        compose_corners(compose_corners(_trans_cube_ ## T, c), \
        _trans_cube_ ## T ## _inverse)
#define _trans_corners_mirrored(T, c) \
        invertco(compose_corners( \
	compose_corners(_trans_cube_ ## T, c), _trans_cube_ ## T ## _inverse))

#define _trans_rotation(T, c) \
        compose(compose(_trans_cube_ ## T, c), \
        _trans_cube_ ## T ## _inverse)
#define _trans_mirrored(T, c) \
        invertco(compose(compose(_trans_cube_ ## T, c), \
        _trans_cube_ ## T ## _inverse))

static cube_t cube_trans_table[48] = {
	[_trans_UFr] = _trans_cube_UFr,
	[_trans_UFm] = _trans_cube_UFm,
	[_trans_ULr] = _trans_cube_URr,
	[_trans_ULm] = _trans_cube_ULm,
	[_trans_UBr] = _trans_cube_UBr,
	[_trans_UBm] = _trans_cube_UBm,
	[_trans_URr] = _trans_cube_ULr,
	[_trans_URm] = _trans_cube_URm,
	[_trans_DFr] = _trans_cube_DFr,
	[_trans_DFm] = _trans_cube_DFm,
	[_trans_DLr] = _trans_cube_DLr,
	[_trans_DLm] = _trans_cube_DRm,
	[_trans_DBr] = _trans_cube_DBr,
	[_trans_DBm] = _trans_cube_DBm,
	[_trans_DRr] = _trans_cube_DRr,
	[_trans_DRm] = _trans_cube_DLm,
	[_trans_RUr] = _trans_cube_FRr,
	[_trans_RUm] = _trans_cube_FLm,
	[_trans_RFr] = _trans_cube_LFr,
	[_trans_RFm] = _trans_cube_RFm,
	[_trans_RDr] = _trans_cube_BLr,
	[_trans_RDm] = _trans_cube_BRm,
	[_trans_RBr] = _trans_cube_RBr,
	[_trans_RBm] = _trans_cube_LBm,
	[_trans_LUr] = _trans_cube_FLr,
	[_trans_LUm] = _trans_cube_FRm,
	[_trans_LFr] = _trans_cube_RFr,
	[_trans_LFm] = _trans_cube_LFm,
	[_trans_LDr] = _trans_cube_BRr,
	[_trans_LDm] = _trans_cube_BLm,
	[_trans_LBr] = _trans_cube_LBr,
	[_trans_LBm] = _trans_cube_RBm,
	[_trans_FUr] = _trans_cube_FUr,
	[_trans_FUm] = _trans_cube_FUm,
	[_trans_FRr] = _trans_cube_RUr,
	[_trans_FRm] = _trans_cube_LUm,
	[_trans_FDr] = _trans_cube_BUr,
	[_trans_FDm] = _trans_cube_BUm,
	[_trans_FLr] = _trans_cube_LUr,
	[_trans_FLm] = _trans_cube_RUm,
	[_trans_BUr] = _trans_cube_FDr,
	[_trans_BUm] = _trans_cube_FDm,
	[_trans_BRr] = _trans_cube_LDr,
	[_trans_BRm] = _trans_cube_RDm,
	[_trans_BDr] = _trans_cube_BDr,
	[_trans_BDm] = _trans_cube_BDm,
	[_trans_BLr] = _trans_cube_RDr,
	[_trans_BLm] = _trans_cube_LDm,
};

static cube_t cube_trans_table_inverse[48] = {
	[_trans_UFr] = _trans_cube_UFr_inverse,
	[_trans_UFm] = _trans_cube_UFm_inverse,
	[_trans_ULr] = _trans_cube_URr_inverse,
	[_trans_ULm] = _trans_cube_ULm_inverse,
	[_trans_UBr] = _trans_cube_UBr_inverse,
	[_trans_UBm] = _trans_cube_UBm_inverse,
	[_trans_URr] = _trans_cube_ULr_inverse,
	[_trans_URm] = _trans_cube_URm_inverse,
	[_trans_DFr] = _trans_cube_DFr_inverse,
	[_trans_DFm] = _trans_cube_DFm_inverse,
	[_trans_DLr] = _trans_cube_DLr_inverse,
	[_trans_DLm] = _trans_cube_DRm_inverse,
	[_trans_DBr] = _trans_cube_DBr_inverse,
	[_trans_DBm] = _trans_cube_DBm_inverse,
	[_trans_DRr] = _trans_cube_DRr_inverse,
	[_trans_DRm] = _trans_cube_DLm_inverse,
	[_trans_RUr] = _trans_cube_FRr_inverse,
	[_trans_RUm] = _trans_cube_FLm_inverse,
	[_trans_RFr] = _trans_cube_LFr_inverse,
	[_trans_RFm] = _trans_cube_RFm_inverse,
	[_trans_RDr] = _trans_cube_BLr_inverse,
	[_trans_RDm] = _trans_cube_BRm_inverse,
	[_trans_RBr] = _trans_cube_RBr_inverse,
	[_trans_RBm] = _trans_cube_LBm_inverse,
	[_trans_LUr] = _trans_cube_FLr_inverse,
	[_trans_LUm] = _trans_cube_FRm_inverse,
	[_trans_LFr] = _trans_cube_RFr_inverse,
	[_trans_LFm] = _trans_cube_LFm_inverse,
	[_trans_LDr] = _trans_cube_BRr_inverse,
	[_trans_LDm] = _trans_cube_BLm_inverse,
	[_trans_LBr] = _trans_cube_LBr_inverse,
	[_trans_LBm] = _trans_cube_RBm_inverse,
	[_trans_FUr] = _trans_cube_FUr_inverse,
	[_trans_FUm] = _trans_cube_FUm_inverse,
	[_trans_FRr] = _trans_cube_RUr_inverse,
	[_trans_FRm] = _trans_cube_LUm_inverse,
	[_trans_FDr] = _trans_cube_BUr_inverse,
	[_trans_FDm] = _trans_cube_BUm_inverse,
	[_trans_FLr] = _trans_cube_LUr_inverse,
	[_trans_FLm] = _trans_cube_RUm_inverse,
	[_trans_BUr] = _trans_cube_FDr_inverse,
	[_trans_BUm] = _trans_cube_FDm_inverse,
	[_trans_BRr] = _trans_cube_LDr_inverse,
	[_trans_BRm] = _trans_cube_RDm_inverse,
	[_trans_BDr] = _trans_cube_BDr_inverse,
	[_trans_BDm] = _trans_cube_BDm_inverse,
	[_trans_BLr] = _trans_cube_RDr_inverse,
	[_trans_BLm] = _trans_cube_LDm_inverse,
};

_static cube_t
transform_edges(cube_t c, uint8_t t)
{
	cube_t ret, trans_cube, trans_inv;

	DBG_ASSERT(t < 48, zero,
	    "transform: invalid transformation %" PRIu8
	    ", must be between 0 and 47\n", t);

	trans_cube = cube_trans_table[t];
	trans_inv = cube_trans_table_inverse[t];

	ret = compose_edges(trans_cube, c);
	ret = compose_edges(ret, trans_inv);

	return ret;
}

_static cube_t
transform_corners(cube_t c, uint8_t t)
{
	cube_t ret, trans_cube, trans_inv;

	DBG_ASSERT(t < 48, zero,
	    "transform: invalid transformation %" PRIu8
	    ", must be between 0 and 47\n", t);

	trans_cube = cube_trans_table[t];
	trans_inv = cube_trans_table_inverse[t];

	ret = compose_corners(trans_cube, c);
	ret = compose_corners(ret, trans_inv);

	return t < 24 ? ret : invertco(ret);
}

_static cube_t
transform(cube_t c, uint8_t t)
{
	cube_t ret, trans_cube, trans_inv;

	DBG_ASSERT(t < 48, zero,
	    "transform: invalid transformation %" PRIu8
	    ", must be between 0 and 47\n", t);

	trans_cube = cube_trans_table[t];
	trans_inv = cube_trans_table_inverse[t];

	ret = compose(trans_cube, c);
	ret = compose(ret, trans_inv);

	return t < 24 ? ret : invertco(ret);
}
