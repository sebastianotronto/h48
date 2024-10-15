#define UINT8_BIT(i) (UINT8_C(1) << (uint8_t)(i))

#define FACTORIAL_MAX INT64_C(12)

#define POW_2_11   INT64_C(2048)
#define POW_3_7    INT64_C(2187)
#define FACT_12    INT64_C(479001600)
#define FACT_8     INT64_C(40320)
#define COMB_12_4  INT64_C(495)
#define COMB_8_4   INT64_C(70)

STATIC int64_t binomial[12][12] = {
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

#define MOVE_U  UINT8_C(0)
#define MOVE_U2 UINT8_C(1)
#define MOVE_U3 UINT8_C(2)
#define MOVE_D  UINT8_C(3)
#define MOVE_D2 UINT8_C(4)
#define MOVE_D3 UINT8_C(5)
#define MOVE_R  UINT8_C(6)
#define MOVE_R2 UINT8_C(7)
#define MOVE_R3 UINT8_C(8)
#define MOVE_L  UINT8_C(9)
#define MOVE_L2 UINT8_C(10)
#define MOVE_L3 UINT8_C(11)
#define MOVE_F  UINT8_C(12)
#define MOVE_F2 UINT8_C(13)
#define MOVE_F3 UINT8_C(14)
#define MOVE_B  UINT8_C(15)
#define MOVE_B2 UINT8_C(16)
#define MOVE_B3 UINT8_C(17)

#define TRANS_UFr UINT8_C(0)
#define TRANS_ULr UINT8_C(1)
#define TRANS_UBr UINT8_C(2)
#define TRANS_URr UINT8_C(3)
#define TRANS_DFr UINT8_C(4)
#define TRANS_DLr UINT8_C(5)
#define TRANS_DBr UINT8_C(6)
#define TRANS_DRr UINT8_C(7)
#define TRANS_RUr UINT8_C(8)
#define TRANS_RFr UINT8_C(9)
#define TRANS_RDr UINT8_C(10)
#define TRANS_RBr UINT8_C(11)
#define TRANS_LUr UINT8_C(12)
#define TRANS_LFr UINT8_C(13)
#define TRANS_LDr UINT8_C(14)
#define TRANS_LBr UINT8_C(15)
#define TRANS_FUr UINT8_C(16)
#define TRANS_FRr UINT8_C(17)
#define TRANS_FDr UINT8_C(18)
#define TRANS_FLr UINT8_C(19)
#define TRANS_BUr UINT8_C(20)
#define TRANS_BRr UINT8_C(21)
#define TRANS_BDr UINT8_C(22)
#define TRANS_BLr UINT8_C(23)

#define TRANS_UFm UINT8_C(24)
#define TRANS_ULm UINT8_C(25)
#define TRANS_UBm UINT8_C(26)
#define TRANS_URm UINT8_C(27)
#define TRANS_DFm UINT8_C(28)
#define TRANS_DLm UINT8_C(29)
#define TRANS_DBm UINT8_C(30)
#define TRANS_DRm UINT8_C(31)
#define TRANS_RUm UINT8_C(32)
#define TRANS_RFm UINT8_C(33)
#define TRANS_RDm UINT8_C(34)
#define TRANS_RBm UINT8_C(35)
#define TRANS_LUm UINT8_C(36)
#define TRANS_LFm UINT8_C(37)
#define TRANS_LDm UINT8_C(38)
#define TRANS_LBm UINT8_C(39)
#define TRANS_FUm UINT8_C(40)
#define TRANS_FRm UINT8_C(41)
#define TRANS_FDm UINT8_C(42)
#define TRANS_FLm UINT8_C(43)
#define TRANS_BUm UINT8_C(44)
#define TRANS_BRm UINT8_C(45)
#define TRANS_BDm UINT8_C(46)
#define TRANS_BLm UINT8_C(47)

#define MM_NORMAL         UINT8_C(0x00)
#define MM_INVERSE        UINT8_C(0x01)
#define MM_INVERSEBRANCH  UINT8_C(0x03)
#define MM_NORMALBRANCH   UINT8_C(0x02)
#define MM_ALLMOVES       UINT32_C(0x3FFFF)
#define MM_NOHALFTURNS    UINT32_C(0x2DB6D)

#define CORNER_UFR      UINT8_C(0)
#define CORNER_UBL      UINT8_C(1)
#define CORNER_DFL      UINT8_C(2)
#define CORNER_DBR      UINT8_C(3)
#define CORNER_UFL      UINT8_C(4)
#define CORNER_UBR      UINT8_C(5)
#define CORNER_DFR      UINT8_C(6)
#define CORNER_DBL      UINT8_C(7)

#define EDGE_UF       UINT8_C(0)
#define EDGE_UB       UINT8_C(1)
#define EDGE_DB       UINT8_C(2)
#define EDGE_DF       UINT8_C(3)
#define EDGE_UR       UINT8_C(4)
#define EDGE_UL       UINT8_C(5)
#define EDGE_DL       UINT8_C(6)
#define EDGE_DR       UINT8_C(7)
#define EDGE_FR       UINT8_C(8)
#define EDGE_FL       UINT8_C(9)
#define EDGE_BL       UINT8_C(10)
#define EDGE_BR       UINT8_C(11)

#define EOSHIFT     UINT8_C(4)
#define COSHIFT     UINT8_C(5)

#define PBITS       UINT8_C(0xF)
#define ESEPBIT_1   UINT8_C(0x4)
#define ESEPBIT_2   UINT8_C(0x8)
#define CSEPBIT     UINT8_C(0x4)
#define EOBIT       UINT8_C(0x10)
#define COBITS      UINT8_C(0xF0)
#define COBITS_2    UINT8_C(0x60)
#define CTWIST_CW   UINT8_C(0x20)
#define CTWIST_CCW  UINT8_C(0x40)
#define EFLIP       UINT8_C(0x10)
#define UINT8_ERROR UINT8_C(0xFF)

STATIC const char *cornerstr[] = {
	[CORNER_UFR] = "UFR",
	[CORNER_UBL] = "UBL",
	[CORNER_DFL] = "DFL",
	[CORNER_DBR] = "DBR",
	[CORNER_UFL] = "UFL",
	[CORNER_UBR] = "UBR",
	[CORNER_DFR] = "DFR",
	[CORNER_DBL] = "DBL"
};

STATIC const char *cornerstralt[] = {
	[CORNER_UFR] = "URF",
	[CORNER_UBL] = "ULB",
	[CORNER_DFL] = "DLF",
	[CORNER_DBR] = "DRB",
	[CORNER_UFL] = "ULF",
	[CORNER_UBR] = "URB",
	[CORNER_DFR] = "DRF",
	[CORNER_DBL] = "DLB"
};

STATIC const char *edgestr[] = {
	[EDGE_UF] = "UF",
	[EDGE_UB] = "UB",
	[EDGE_DB] = "DB",
	[EDGE_DF] = "DF",
	[EDGE_UR] = "UR",
	[EDGE_UL] = "UL",
	[EDGE_DL] = "DL",
	[EDGE_DR] = "DR",
	[EDGE_FR] = "FR",
	[EDGE_FL] = "FL",
	[EDGE_BL] = "BL",
	[EDGE_BR] = "BR"
};

STATIC const char *movestr[] = {
	[MOVE_U]  = "U",
	[MOVE_U2] = "U2",
	[MOVE_U3] = "U'",
	[MOVE_D]  = "D",
	[MOVE_D2] = "D2",
	[MOVE_D3] = "D'",
	[MOVE_R]  = "R",
	[MOVE_R2] = "R2",
	[MOVE_R3] = "R'",
	[MOVE_L]  = "L",
	[MOVE_L2] = "L2",
	[MOVE_L3] = "L'",
	[MOVE_F]  = "F",
	[MOVE_F2] = "F2",
	[MOVE_F3] = "F'",
	[MOVE_B]  = "B",
	[MOVE_B2] = "B2",
	[MOVE_B3] = "B'",
};

STATIC const char *transstr[] = {
	[TRANS_UFr] = "rotation UF",
	[TRANS_UFm] = "mirrored UF",
	[TRANS_ULr] = "rotation UL",
	[TRANS_ULm] = "mirrored UL",
	[TRANS_UBr] = "rotation UB",
	[TRANS_UBm] = "mirrored UB",
	[TRANS_URr] = "rotation UR",
	[TRANS_URm] = "mirrored UR",
	[TRANS_DFr] = "rotation DF",
	[TRANS_DFm] = "mirrored DF",
	[TRANS_DLr] = "rotation DL",
	[TRANS_DLm] = "mirrored DL",
	[TRANS_DBr] = "rotation DB",
	[TRANS_DBm] = "mirrored DB",
	[TRANS_DRr] = "rotation DR",
	[TRANS_DRm] = "mirrored DR",
	[TRANS_RUr] = "rotation RU",
	[TRANS_RUm] = "mirrored RU",
	[TRANS_RFr] = "rotation RF",
	[TRANS_RFm] = "mirrored RF",
	[TRANS_RDr] = "rotation RD",
	[TRANS_RDm] = "mirrored RD",
	[TRANS_RBr] = "rotation RB",
	[TRANS_RBm] = "mirrored RB",
	[TRANS_LUr] = "rotation LU",
	[TRANS_LUm] = "mirrored LU",
	[TRANS_LFr] = "rotation LF",
	[TRANS_LFm] = "mirrored LF",
	[TRANS_LDr] = "rotation LD",
	[TRANS_LDm] = "mirrored LD",
	[TRANS_LBr] = "rotation LB",
	[TRANS_LBm] = "mirrored LB",
	[TRANS_FUr] = "rotation FU",
	[TRANS_FUm] = "mirrored FU",
	[TRANS_FRr] = "rotation FR",
	[TRANS_FRm] = "mirrored FR",
	[TRANS_FDr] = "rotation FD",
	[TRANS_FDm] = "mirrored FD",
	[TRANS_FLr] = "rotation FL",
	[TRANS_FLm] = "mirrored FL",
	[TRANS_BUr] = "rotation BU",
	[TRANS_BUm] = "mirrored BU",
	[TRANS_BRr] = "rotation BR",
	[TRANS_BRm] = "mirrored BR",
	[TRANS_BDr] = "rotation BD",
	[TRANS_BDm] = "mirrored BD",
	[TRANS_BLr] = "rotation BL",
	[TRANS_BLm] = "mirrored BL",
};

static uint8_t inverse_trans_table[48] = {
	[TRANS_UFr] = TRANS_UFr,
	[TRANS_UFm] = TRANS_UFm,
	[TRANS_ULr] = TRANS_URr,
	[TRANS_ULm] = TRANS_ULm,
	[TRANS_UBr] = TRANS_UBr,
	[TRANS_UBm] = TRANS_UBm,
	[TRANS_URr] = TRANS_ULr,
	[TRANS_URm] = TRANS_URm,
	[TRANS_DFr] = TRANS_DFr,
	[TRANS_DFm] = TRANS_DFm,
	[TRANS_DLr] = TRANS_DLr,
	[TRANS_DLm] = TRANS_DRm,
	[TRANS_DBr] = TRANS_DBr,
	[TRANS_DBm] = TRANS_DBm,
	[TRANS_DRr] = TRANS_DRr,
	[TRANS_DRm] = TRANS_DLm,
	[TRANS_RUr] = TRANS_FRr,
	[TRANS_RUm] = TRANS_FLm,
	[TRANS_RFr] = TRANS_LFr,
	[TRANS_RFm] = TRANS_RFm,
	[TRANS_RDr] = TRANS_BLr,
	[TRANS_RDm] = TRANS_BRm,
	[TRANS_RBr] = TRANS_RBr,
	[TRANS_RBm] = TRANS_LBm,
	[TRANS_LUr] = TRANS_FLr,
	[TRANS_LUm] = TRANS_FRm,
	[TRANS_LFr] = TRANS_RFr,
	[TRANS_LFm] = TRANS_LFm,
	[TRANS_LDr] = TRANS_BRr,
	[TRANS_LDm] = TRANS_BLm,
	[TRANS_LBr] = TRANS_LBr,
	[TRANS_LBm] = TRANS_RBm,
	[TRANS_FUr] = TRANS_FUr,
	[TRANS_FUm] = TRANS_FUm,
	[TRANS_FRr] = TRANS_RUr,
	[TRANS_FRm] = TRANS_LUm,
	[TRANS_FDr] = TRANS_BUr,
	[TRANS_FDm] = TRANS_BUm,
	[TRANS_FLr] = TRANS_LUr,
	[TRANS_FLm] = TRANS_RUm,
	[TRANS_BUr] = TRANS_FDr,
	[TRANS_BUm] = TRANS_FDm,
	[TRANS_BRr] = TRANS_LDr,
	[TRANS_BRm] = TRANS_RDm,
	[TRANS_BDr] = TRANS_BDr,
	[TRANS_BDm] = TRANS_BDm,
	[TRANS_BLr] = TRANS_RDr,
	[TRANS_BLm] = TRANS_LDm,
};
