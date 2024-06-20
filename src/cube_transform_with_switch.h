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

_static cube_t
transform_edges(cube_t c, uint8_t t)
{
	switch (t) {
	case _trans_UFr:
		return _trans_edges_rotation(UFr, c);
	case _trans_ULr:
		return _trans_edges_rotation(ULr, c);
	case _trans_UBr:
		return _trans_edges_rotation(UBr, c);
	case _trans_URr:
		return _trans_edges_rotation(URr, c);
	case _trans_DFr:
		return _trans_edges_rotation(DFr, c);
	case _trans_DLr:
		return _trans_edges_rotation(DLr, c);
	case _trans_DBr:
		return _trans_edges_rotation(DBr, c);
	case _trans_DRr:
		return _trans_edges_rotation(DRr, c);
	case _trans_RUr:
		return _trans_edges_rotation(RUr, c);
	case _trans_RFr:
		return _trans_edges_rotation(RFr, c);
	case _trans_RDr:
		return _trans_edges_rotation(RDr, c);
	case _trans_RBr:
		return _trans_edges_rotation(RBr, c);
	case _trans_LUr:
		return _trans_edges_rotation(LUr, c);
	case _trans_LFr:
		return _trans_edges_rotation(LFr, c);
	case _trans_LDr:
		return _trans_edges_rotation(LDr, c);
	case _trans_LBr:
		return _trans_edges_rotation(LBr, c);
	case _trans_FUr:
		return _trans_edges_rotation(FUr, c);
	case _trans_FRr:
		return _trans_edges_rotation(FRr, c);
	case _trans_FDr:
		return _trans_edges_rotation(FDr, c);
	case _trans_FLr:
		return _trans_edges_rotation(FLr, c);
	case _trans_BUr:
		return _trans_edges_rotation(BUr, c);
	case _trans_BRr:
		return _trans_edges_rotation(BRr, c);
	case _trans_BDr:
		return _trans_edges_rotation(BDr, c);
	case _trans_BLr:
		return _trans_edges_rotation(BLr, c);
	case _trans_UFm:
		return _trans_edges_mirrored(UFm, c);
	case _trans_ULm:
		return _trans_edges_mirrored(ULm, c);
	case _trans_UBm:
		return _trans_edges_mirrored(UBm, c);
	case _trans_URm:
		return _trans_edges_mirrored(URm, c);
	case _trans_DFm:
		return _trans_edges_mirrored(DFm, c);
	case _trans_DLm:
		return _trans_edges_mirrored(DLm, c);
	case _trans_DBm:
		return _trans_edges_mirrored(DBm, c);
	case _trans_DRm:
		return _trans_edges_mirrored(DRm, c);
	case _trans_RUm:
		return _trans_edges_mirrored(RUm, c);
	case _trans_RFm:
		return _trans_edges_mirrored(RFm, c);
	case _trans_RDm:
		return _trans_edges_mirrored(RDm, c);
	case _trans_RBm:
		return _trans_edges_mirrored(RBm, c);
	case _trans_LUm:
		return _trans_edges_mirrored(LUm, c);
	case _trans_LFm:
		return _trans_edges_mirrored(LFm, c);
	case _trans_LDm:
		return _trans_edges_mirrored(LDm, c);
	case _trans_LBm:
		return _trans_edges_mirrored(LBm, c);
	case _trans_FUm:
		return _trans_edges_mirrored(FUm, c);
	case _trans_FRm:
		return _trans_edges_mirrored(FRm, c);
	case _trans_FDm:
		return _trans_edges_mirrored(FDm, c);
	case _trans_FLm:
		return _trans_edges_mirrored(FLm, c);
	case _trans_BUm:
		return _trans_edges_mirrored(BUm, c);
	case _trans_BRm:
		return _trans_edges_mirrored(BRm, c);
	case _trans_BDm:
		return _trans_edges_mirrored(BDm, c);
	case _trans_BLm:
		return _trans_edges_mirrored(BLm, c);
	default:
		LOG("transform error, unknown transformation\n");
		return zero;
	}
}

_static cube_t
transform_corners(cube_t c, uint8_t t)
{
	switch (t) {
	case _trans_UFr:
		return _trans_corners_rotation(UFr, c);
	case _trans_ULr:
		return _trans_corners_rotation(ULr, c);
	case _trans_UBr:
		return _trans_corners_rotation(UBr, c);
	case _trans_URr:
		return _trans_corners_rotation(URr, c);
	case _trans_DFr:
		return _trans_corners_rotation(DFr, c);
	case _trans_DLr:
		return _trans_corners_rotation(DLr, c);
	case _trans_DBr:
		return _trans_corners_rotation(DBr, c);
	case _trans_DRr:
		return _trans_corners_rotation(DRr, c);
	case _trans_RUr:
		return _trans_corners_rotation(RUr, c);
	case _trans_RFr:
		return _trans_corners_rotation(RFr, c);
	case _trans_RDr:
		return _trans_corners_rotation(RDr, c);
	case _trans_RBr:
		return _trans_corners_rotation(RBr, c);
	case _trans_LUr:
		return _trans_corners_rotation(LUr, c);
	case _trans_LFr:
		return _trans_corners_rotation(LFr, c);
	case _trans_LDr:
		return _trans_corners_rotation(LDr, c);
	case _trans_LBr:
		return _trans_corners_rotation(LBr, c);
	case _trans_FUr:
		return _trans_corners_rotation(FUr, c);
	case _trans_FRr:
		return _trans_corners_rotation(FRr, c);
	case _trans_FDr:
		return _trans_corners_rotation(FDr, c);
	case _trans_FLr:
		return _trans_corners_rotation(FLr, c);
	case _trans_BUr:
		return _trans_corners_rotation(BUr, c);
	case _trans_BRr:
		return _trans_corners_rotation(BRr, c);
	case _trans_BDr:
		return _trans_corners_rotation(BDr, c);
	case _trans_BLr:
		return _trans_corners_rotation(BLr, c);
	case _trans_UFm:
		return _trans_corners_mirrored(UFm, c);
	case _trans_ULm:
		return _trans_corners_mirrored(ULm, c);
	case _trans_UBm:
		return _trans_corners_mirrored(UBm, c);
	case _trans_URm:
		return _trans_corners_mirrored(URm, c);
	case _trans_DFm:
		return _trans_corners_mirrored(DFm, c);
	case _trans_DLm:
		return _trans_corners_mirrored(DLm, c);
	case _trans_DBm:
		return _trans_corners_mirrored(DBm, c);
	case _trans_DRm:
		return _trans_corners_mirrored(DRm, c);
	case _trans_RUm:
		return _trans_corners_mirrored(RUm, c);
	case _trans_RFm:
		return _trans_corners_mirrored(RFm, c);
	case _trans_RDm:
		return _trans_corners_mirrored(RDm, c);
	case _trans_RBm:
		return _trans_corners_mirrored(RBm, c);
	case _trans_LUm:
		return _trans_corners_mirrored(LUm, c);
	case _trans_LFm:
		return _trans_corners_mirrored(LFm, c);
	case _trans_LDm:
		return _trans_corners_mirrored(LDm, c);
	case _trans_LBm:
		return _trans_corners_mirrored(LBm, c);
	case _trans_FUm:
		return _trans_corners_mirrored(FUm, c);
	case _trans_FRm:
		return _trans_corners_mirrored(FRm, c);
	case _trans_FDm:
		return _trans_corners_mirrored(FDm, c);
	case _trans_FLm:
		return _trans_corners_mirrored(FLm, c);
	case _trans_BUm:
		return _trans_corners_mirrored(BUm, c);
	case _trans_BRm:
		return _trans_corners_mirrored(BRm, c);
	case _trans_BDm:
		return _trans_corners_mirrored(BDm, c);
	case _trans_BLm:
		return _trans_corners_mirrored(BLm, c);
	default:
		LOG("transform error, unknown transformation\n");
		return zero;
	}
}

_static cube_t
transform(cube_t c, uint8_t t)
{
	switch (t) {
	case _trans_UFr:
		return _trans_rotation(UFr, c);
	case _trans_ULr:
		return _trans_rotation(ULr, c);
	case _trans_UBr:
		return _trans_rotation(UBr, c);
	case _trans_URr:
		return _trans_rotation(URr, c);
	case _trans_DFr:
		return _trans_rotation(DFr, c);
	case _trans_DLr:
		return _trans_rotation(DLr, c);
	case _trans_DBr:
		return _trans_rotation(DBr, c);
	case _trans_DRr:
		return _trans_rotation(DRr, c);
	case _trans_RUr:
		return _trans_rotation(RUr, c);
	case _trans_RFr:
		return _trans_rotation(RFr, c);
	case _trans_RDr:
		return _trans_rotation(RDr, c);
	case _trans_RBr:
		return _trans_rotation(RBr, c);
	case _trans_LUr:
		return _trans_rotation(LUr, c);
	case _trans_LFr:
		return _trans_rotation(LFr, c);
	case _trans_LDr:
		return _trans_rotation(LDr, c);
	case _trans_LBr:
		return _trans_rotation(LBr, c);
	case _trans_FUr:
		return _trans_rotation(FUr, c);
	case _trans_FRr:
		return _trans_rotation(FRr, c);
	case _trans_FDr:
		return _trans_rotation(FDr, c);
	case _trans_FLr:
		return _trans_rotation(FLr, c);
	case _trans_BUr:
		return _trans_rotation(BUr, c);
	case _trans_BRr:
		return _trans_rotation(BRr, c);
	case _trans_BDr:
		return _trans_rotation(BDr, c);
	case _trans_BLr:
		return _trans_rotation(BLr, c);
	case _trans_UFm:
		return _trans_mirrored(UFm, c);
	case _trans_ULm:
		return _trans_mirrored(ULm, c);
	case _trans_UBm:
		return _trans_mirrored(UBm, c);
	case _trans_URm:
		return _trans_mirrored(URm, c);
	case _trans_DFm:
		return _trans_mirrored(DFm, c);
	case _trans_DLm:
		return _trans_mirrored(DLm, c);
	case _trans_DBm:
		return _trans_mirrored(DBm, c);
	case _trans_DRm:
		return _trans_mirrored(DRm, c);
	case _trans_RUm:
		return _trans_mirrored(RUm, c);
	case _trans_RFm:
		return _trans_mirrored(RFm, c);
	case _trans_RDm:
		return _trans_mirrored(RDm, c);
	case _trans_RBm:
		return _trans_mirrored(RBm, c);
	case _trans_LUm:
		return _trans_mirrored(LUm, c);
	case _trans_LFm:
		return _trans_mirrored(LFm, c);
	case _trans_LDm:
		return _trans_mirrored(LDm, c);
	case _trans_LBm:
		return _trans_mirrored(LBm, c);
	case _trans_FUm:
		return _trans_mirrored(FUm, c);
	case _trans_FRm:
		return _trans_mirrored(FRm, c);
	case _trans_FDm:
		return _trans_mirrored(FDm, c);
	case _trans_FLm:
		return _trans_mirrored(FLm, c);
	case _trans_BUm:
		return _trans_mirrored(BUm, c);
	case _trans_BRm:
		return _trans_mirrored(BRm, c);
	case _trans_BDm:
		return _trans_mirrored(BDm, c);
	case _trans_BLm:
		return _trans_mirrored(BLm, c);
	default:
		LOG("transform error, unknown transformation\n");
		return zero;
	}
}
