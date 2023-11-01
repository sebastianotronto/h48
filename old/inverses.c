static move_t inverse_move_arr[] = {
	[U]  = U3,
	[U2] = U2,
	[U3] = U,
	[D]  = D3,
	[D2] = D2,
	[D3] = D,
	[R]  = R3,
	[R2] = R2,
	[R3] = R,
	[L]  = L3,
	[L2] = L2,
	[L3] = L,
	[F]  = F3,
	[F2] = F2,
	[F3] = F,
	[B]  = B3,
	[B2] = B2,
	[B3] = B,
};

static trans_t inverse_trans_arr[] = {
	[UFr] = UFr,
	[ULr] = URr,
	[UBr] = UBr,
	[URr] = URr,
	[DFr] = DFr,
	[DLr] = DLr,
	[DRr] = DRr,
	[DBr] = DBr,
	[RUr] = FRr,
	[RFr] = LFr,
	[RDr] = BLr,
	[RBr] = RBr,
	[LUr] = FLr,
	[LFr] = RFr,
	[LDr] = BRr,
	[LBr] = LBr,
	[FUr] = FUr,
	[FRr] = RUr,
	[FDr] = BUr,
	[FLr] = LUr,
	[BUr] = FDr,
	[BLr] = RDr,
	[BDr] = BDr,
	[BRr] = LDr,
	[UFm] = UFm,
	[ULm] = ULm,
	[UBm] = UBm,
	[URm] = ULm,
	[DFm] = DFm,
	[DLm] = DRm,
	[DRm] = DLm,
	[DBm] = DBm,
	[RUm] = FLm,
	[RFm] = RFm,
	[RDm] = BRm,
	[RBm] = LBm,
	[LUm] = FRm,
	[LFm] = LFm,
	[LDm] = BLm,
	[LBm] = RBm,
	[FUm] = FUm,
	[FRm] = LUm,
	[FDm] = BUm,
	[FLm] = RUm,
	[BUm] = FDm,
	[BLm] = LDm,
	[BDm] = BDm,
	[BRm] = RDm,
};