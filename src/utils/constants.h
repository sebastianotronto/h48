#define _bit_u8(i)  (UINT8_C(1) << (uint8_t)(i))
#define _bit_u32(i) (UINT32_C(1) << (uint32_t)(i))
#define _bit_u64(i) (UINT64_C(1) << (uint64_t)(i))

#define _max_factorial INT64_C(12)

#define _2p11  INT64_C(2048)
#define _2p12  INT64_C(4096)
#define _3p7   INT64_C(2187)
#define _3p8   INT64_C(6561)
#define _12f   INT64_C(479001600)
#define _8f    INT64_C(40320)
#define _12c4  INT64_C(495)
#define _8c4   INT64_C(70)

_static int64_t binomial[12][12] = {
	{1,  0,  0,   0,   0,   0,   0,   0,   0,  0,  0, 0},
	{1,  1,  0,   0,   0,   0,   0,   0,   0,  0,  0, 0},
	{1,  2,  1,   0,   0,   0,   0,   0,   0,  0,  0, 0},
	{1,  3,  3,   1,   0,   0,   0,   0,   0,  0,  0, 0},
	{1,  4,  6,   4,   1,   0,   0,   0,   0,  0,  0, 0},
	{1,  5, 10,  10,   5,   1,   0,   0,   0,  0,  0, 0},
	{1,  6, 15,  20,  15,   6,   1,   0,   0,  0,  0, 0},
	{1,  7, 21,  35,  35,  21,   7,   1,   0,  0,  0, 0},
	{1,  8, 28,  56,  70,  56,  28,   8,   1,  0,  0, 0},
	{1,  9, 36,  84, 126, 126,  84,  36,   9,  1,  0, 0},
	{1, 10, 45, 120, 210, 252, 210, 120,  45, 10,  1, 0},
	{1, 11, 55, 165, 330, 462, 462, 330, 165, 55, 11, 1},
};

#define _move_U  UINT8_C(0)
#define _move_U2 UINT8_C(1)
#define _move_U3 UINT8_C(2)
#define _move_D  UINT8_C(3)
#define _move_D2 UINT8_C(4)
#define _move_D3 UINT8_C(5)
#define _move_R  UINT8_C(6)
#define _move_R2 UINT8_C(7)
#define _move_R3 UINT8_C(8)
#define _move_L  UINT8_C(9)
#define _move_L2 UINT8_C(10)
#define _move_L3 UINT8_C(11)
#define _move_F  UINT8_C(12)
#define _move_F2 UINT8_C(13)
#define _move_F3 UINT8_C(14)
#define _move_B  UINT8_C(15)
#define _move_B2 UINT8_C(16)
#define _move_B3 UINT8_C(17)

#define _trans_UFr UINT8_C(0)
#define _trans_ULr UINT8_C(1)
#define _trans_UBr UINT8_C(2)
#define _trans_URr UINT8_C(3)
#define _trans_DFr UINT8_C(4)
#define _trans_DLr UINT8_C(5)
#define _trans_DBr UINT8_C(6)
#define _trans_DRr UINT8_C(7)
#define _trans_RUr UINT8_C(8)
#define _trans_RFr UINT8_C(9)
#define _trans_RDr UINT8_C(10)
#define _trans_RBr UINT8_C(11)
#define _trans_LUr UINT8_C(12)
#define _trans_LFr UINT8_C(13)
#define _trans_LDr UINT8_C(14)
#define _trans_LBr UINT8_C(15)
#define _trans_FUr UINT8_C(16)
#define _trans_FRr UINT8_C(17)
#define _trans_FDr UINT8_C(18)
#define _trans_FLr UINT8_C(19)
#define _trans_BUr UINT8_C(20)
#define _trans_BRr UINT8_C(21)
#define _trans_BDr UINT8_C(22)
#define _trans_BLr UINT8_C(23)

#define _trans_UFm UINT8_C(24)
#define _trans_ULm UINT8_C(25)
#define _trans_UBm UINT8_C(26)
#define _trans_URm UINT8_C(27)
#define _trans_DFm UINT8_C(28)
#define _trans_DLm UINT8_C(29)
#define _trans_DBm UINT8_C(30)
#define _trans_DRm UINT8_C(31)
#define _trans_RUm UINT8_C(32)
#define _trans_RFm UINT8_C(33)
#define _trans_RDm UINT8_C(34)
#define _trans_RBm UINT8_C(35)
#define _trans_LUm UINT8_C(36)
#define _trans_LFm UINT8_C(37)
#define _trans_LDm UINT8_C(38)
#define _trans_LBm UINT8_C(39)
#define _trans_FUm UINT8_C(40)
#define _trans_FRm UINT8_C(41)
#define _trans_FDm UINT8_C(42)
#define _trans_FLm UINT8_C(43)
#define _trans_BUm UINT8_C(44)
#define _trans_BRm UINT8_C(45)
#define _trans_BDm UINT8_C(46)
#define _trans_BLm UINT8_C(47)

#define _mm_normal         UINT32_C(0x00)
#define _mm_inverse        UINT32_C(0x01)
#define _mm_inversebranch  UINT32_C(0x03)
#define _mm_normalbranch   UINT32_C(0x02)
#define _mm_allmoves       UINT32_C(0x3FFFF)
#define _mm_nohalfturns    UINT32_C(0x2DB6D)

#define _c_ufr      UINT8_C(0)
#define _c_ubl      UINT8_C(1)
#define _c_dfl      UINT8_C(2)
#define _c_dbr      UINT8_C(3)
#define _c_ufl      UINT8_C(4)
#define _c_ubr      UINT8_C(5)
#define _c_dfr      UINT8_C(6)
#define _c_dbl      UINT8_C(7)

#define _e_uf       UINT8_C(0)
#define _e_ub       UINT8_C(1)
#define _e_db       UINT8_C(2)
#define _e_df       UINT8_C(3)
#define _e_ur       UINT8_C(4)
#define _e_ul       UINT8_C(5)
#define _e_dl       UINT8_C(6)
#define _e_dr       UINT8_C(7)
#define _e_fr       UINT8_C(8)
#define _e_fl       UINT8_C(9)
#define _e_bl       UINT8_C(10)
#define _e_br       UINT8_C(11)

#define _eoshift    UINT8_C(4)
#define _coshift    UINT8_C(5)

#define _pbits      UINT8_C(0xF)
#define _esepbit1   UINT8_C(0x4)
#define _esepbit2   UINT8_C(0x8)
#define _csepbit    UINT8_C(0x4)
#define _eobit      UINT8_C(0x10)
#define _cobits     UINT8_C(0xF0)
#define _cobits2    UINT8_C(0x60)
#define _ctwist_cw  UINT8_C(0x20)
#define _ctwist_ccw UINT8_C(0x40)
#define _eflip      UINT8_C(0x10)
#define _error      UINT8_C(0xFF)

_static const char *cornerstr[] = {
	[_c_ufr] = "UFR",
	[_c_ubl] = "UBL",
	[_c_dfl] = "DFL",
	[_c_dbr] = "DBR",
	[_c_ufl] = "UFL",
	[_c_ubr] = "UBR",
	[_c_dfr] = "DFR",
	[_c_dbl] = "DBL"
};

_static const char *cornerstralt[] = {
	[_c_ufr] = "URF",
	[_c_ubl] = "ULB",
	[_c_dfl] = "DLF",
	[_c_dbr] = "DRB",
	[_c_ufl] = "ULF",
	[_c_ubr] = "URB",
	[_c_dfr] = "DRF",
	[_c_dbl] = "DLB"
};

_static const char *edgestr[] = {
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

_static const char *movestr[] = {
	[_move_U]  = "U",
	[_move_U2] = "U2",
	[_move_U3] = "U'",
	[_move_D]  = "D",
	[_move_D2] = "D2",
	[_move_D3] = "D'",
	[_move_R]  = "R",
	[_move_R2] = "R2",
	[_move_R3] = "R'",
	[_move_L]  = "L",
	[_move_L2] = "L2",
	[_move_L3] = "L'",
	[_move_F]  = "F",
	[_move_F2] = "F2",
	[_move_F3] = "F'",
	[_move_B]  = "B",
	[_move_B2] = "B2",
	[_move_B3] = "B'",
};

_static const char *transstr[] = {
	[_trans_UFr] = "rotation UF",
	[_trans_UFm] = "mirrored UF",
	[_trans_ULr] = "rotation UL",
	[_trans_ULm] = "mirrored UL",
	[_trans_UBr] = "rotation UB",
	[_trans_UBm] = "mirrored UB",
	[_trans_URr] = "rotation UR",
	[_trans_URm] = "mirrored UR",
	[_trans_DFr] = "rotation DF",
	[_trans_DFm] = "mirrored DF",
	[_trans_DLr] = "rotation DL",
	[_trans_DLm] = "mirrored DL",
	[_trans_DBr] = "rotation DB",
	[_trans_DBm] = "mirrored DB",
	[_trans_DRr] = "rotation DR",
	[_trans_DRm] = "mirrored DR",
	[_trans_RUr] = "rotation RU",
	[_trans_RUm] = "mirrored RU",
	[_trans_RFr] = "rotation RF",
	[_trans_RFm] = "mirrored RF",
	[_trans_RDr] = "rotation RD",
	[_trans_RDm] = "mirrored RD",
	[_trans_RBr] = "rotation RB",
	[_trans_RBm] = "mirrored RB",
	[_trans_LUr] = "rotation LU",
	[_trans_LUm] = "mirrored LU",
	[_trans_LFr] = "rotation LF",
	[_trans_LFm] = "mirrored LF",
	[_trans_LDr] = "rotation LD",
	[_trans_LDm] = "mirrored LD",
	[_trans_LBr] = "rotation LB",
	[_trans_LBm] = "mirrored LB",
	[_trans_FUr] = "rotation FU",
	[_trans_FUm] = "mirrored FU",
	[_trans_FRr] = "rotation FR",
	[_trans_FRm] = "mirrored FR",
	[_trans_FDr] = "rotation FD",
	[_trans_FDm] = "mirrored FD",
	[_trans_FLr] = "rotation FL",
	[_trans_FLm] = "mirrored FL",
	[_trans_BUr] = "rotation BU",
	[_trans_BUm] = "mirrored BU",
	[_trans_BRr] = "rotation BR",
	[_trans_BRm] = "mirrored BR",
	[_trans_BDr] = "rotation BD",
	[_trans_BDm] = "mirrored BD",
	[_trans_BLr] = "rotation BL",
	[_trans_BLm] = "mirrored BL",
};

static uint8_t inverse_trans_table[48] = {
	[_trans_UFr] = _trans_UFr,
	[_trans_UFm] = _trans_UFm,
	[_trans_ULr] = _trans_URr,
	[_trans_ULm] = _trans_ULm,
	[_trans_UBr] = _trans_UBr,
	[_trans_UBm] = _trans_UBm,
	[_trans_URr] = _trans_ULr,
	[_trans_URm] = _trans_URm,
	[_trans_DFr] = _trans_DFr,
	[_trans_DFm] = _trans_DFm,
	[_trans_DLr] = _trans_DLr,
	[_trans_DLm] = _trans_DRm,
	[_trans_DBr] = _trans_DBr,
	[_trans_DBm] = _trans_DBm,
	[_trans_DRr] = _trans_DRr,
	[_trans_DRm] = _trans_DLm,
	[_trans_RUr] = _trans_FRr,
	[_trans_RUm] = _trans_FLm,
	[_trans_RFr] = _trans_LFr,
	[_trans_RFm] = _trans_RFm,
	[_trans_RDr] = _trans_BLr,
	[_trans_RDm] = _trans_BRm,
	[_trans_RBr] = _trans_RBr,
	[_trans_RBm] = _trans_LBm,
	[_trans_LUr] = _trans_FLr,
	[_trans_LUm] = _trans_FRm,
	[_trans_LFr] = _trans_RFr,
	[_trans_LFm] = _trans_LFm,
	[_trans_LDr] = _trans_BRr,
	[_trans_LDm] = _trans_BLm,
	[_trans_LBr] = _trans_LBr,
	[_trans_LBm] = _trans_RBm,
	[_trans_FUr] = _trans_FUr,
	[_trans_FUm] = _trans_FUm,
	[_trans_FRr] = _trans_RUr,
	[_trans_FRm] = _trans_LUm,
	[_trans_FDr] = _trans_BUr,
	[_trans_FDm] = _trans_BUm,
	[_trans_FLr] = _trans_LUr,
	[_trans_FLm] = _trans_RUm,
	[_trans_BUr] = _trans_FDr,
	[_trans_BUm] = _trans_FDm,
	[_trans_BRr] = _trans_LDr,
	[_trans_BRm] = _trans_RDm,
	[_trans_BDr] = _trans_BDr,
	[_trans_BDm] = _trans_BDm,
	[_trans_BLr] = _trans_RDr,
	[_trans_BLm] = _trans_LDm,
};
