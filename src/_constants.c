#define _c_ufr      0U
#define _c_ubl      1U
#define _c_dfl      2U
#define _c_dbr      3U
#define _c_ufl      4U
#define _c_ubr      5U
#define _c_dfr      6U
#define _c_dbl      7U

#define _e_uf       0U
#define _e_ub       1U
#define _e_db       2U
#define _e_df       3U
#define _e_ur       4U
#define _e_ul       5U
#define _e_dl       6U
#define _e_dr       7U
#define _e_fr       8U
#define _e_fl       9U
#define _e_bl       10U
#define _e_br       11U

#define _eoshift    4U
#define _coshift    5U

#define _pbits      0xFU
#define _eobit      0x10U
#define _cobits     0xF0U
#define _cobits2    0x60U
#define _ctwist_cw  0x20U
#define _ctwist_ccw 0x40U
#define _eflip      0x10U
#define _error      0xFFU

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

static char *cornerstr[] = {
	[_c_ufr] = "UFR",
	[_c_ubl] = "UBL",
	[_c_dfl] = "DFL",
	[_c_dbr] = "DBR",
	[_c_ufl] = "UFL",
	[_c_ubr] = "UBR",
	[_c_dfr] = "DFR",
	[_c_dbl] = "DBL"
};

static char *cornerstralt[] = {
	[_c_ufr] = "URF",
	[_c_ubl] = "ULB",
	[_c_dfl] = "DLF",
	[_c_dbr] = "DRB",
	[_c_ufl] = "ULF",
	[_c_ubr] = "URB",
	[_c_dfr] = "DRF",
	[_c_dbl] = "DLB"
};

static char *edgestr[] = {
	[_e_uf] = "UF",
	[_e_ub] = "UB",
	[_e_db] = "DB",
	[_e_df] = "DF",
	[_e_ur] = "UR",
	[_e_ul] = "UL",
	[_e_dl] = "DL",
	[_e_dr] = "DR",
	[_e_fr] = "FR",
	[_e_fl] = "FL",
	[_e_bl] = "BL",
	[_e_br] = "BR"
};

static char *movestr[] = {
	[U]  = "U",
	[U2] = "U2",
	[U3] = "U'",
	[D]  = "D",
	[D2] = "D2",
	[D3] = "D'",
	[R]  = "R",
	[R2] = "R2",
	[R3] = "R'",
	[L]  = "L",
	[L2] = "L2",
	[L3] = "L'",
	[F]  = "F",
	[F2] = "F2",
	[F3] = "F'",
	[B]  = "B",
	[B2] = "B2",
	[B3] = "B'",
};

static char *transstr[] = {
	[UFr] = "rotation UF",
	[UFm] = "mirrored UF",
	[ULr] = "rotation UL",
	[ULm] = "mirrored UL",
	[UBr] = "rotation UB",
	[UBm] = "mirrored UB",
	[URr] = "rotation UR",
	[URm] = "mirrored UR",
	[DFr] = "rotation DF",
	[DFm] = "mirrored DF",
	[DLr] = "rotation DL",
	[DLm] = "mirrored DL",
	[DBr] = "rotation DB",
	[DBm] = "mirrored DB",
	[DRr] = "rotation DR",
	[DRm] = "mirrored DR",
	[RUr] = "rotation RU",
	[RUm] = "mirrored RU",
	[RFr] = "rotation RF",
	[RFm] = "mirrored RF",
	[RDr] = "rotation RD",
	[RDm] = "mirrored RD",
	[RBr] = "rotation RB",
	[RBm] = "mirrored RB",
	[LUr] = "rotation LU",
	[LUm] = "mirrored LU",
	[LFr] = "rotation LF",
	[LFm] = "mirrored LF",
	[LDr] = "rotation LD",
	[LDm] = "mirrored LD",
	[LBr] = "rotation LB",
	[LBm] = "mirrored LB",
	[FUr] = "rotation FU",
	[FUm] = "mirrored FU",
	[FRr] = "rotation FR",
	[FRm] = "mirrored FR",
	[FDr] = "rotation FD",
	[FDm] = "mirrored FD",
	[FLr] = "rotation FL",
	[FLm] = "mirrored FL",
	[BUr] = "rotation BU",
	[BUm] = "mirrored BU",
	[BRr] = "rotation BR",
	[BRm] = "mirrored BR",
	[BDr] = "rotation BD",
	[BDm] = "mirrored BD",
	[BLr] = "rotation BL",
	[BLm] = "mirrored BL",
};
