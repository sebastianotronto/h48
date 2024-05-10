#define _2p11  2048U
#define _2p12  4096U
#define _3p7   2187U
#define _3p8   6561U
#define _12c4  495U
#define _8c4   70U

#define COCSEP_CLASSES 3393U

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

#define _move_U  0U
#define _move_U2 1U
#define _move_U3 2U
#define _move_D  3U
#define _move_D2 4U
#define _move_D3 5U
#define _move_R  6U
#define _move_R2 7U
#define _move_R3 8U
#define _move_L  9U
#define _move_L2 10U
#define _move_L3 11U
#define _move_F  12U
#define _move_F2 13U
#define _move_F3 14U
#define _move_B  15U
#define _move_B2 16U
#define _move_B3 17U

#define _trans_UFr 0
#define _trans_ULr 1
#define _trans_UBr 2
#define _trans_URr 3
#define _trans_DFr 4
#define _trans_DLr 5
#define _trans_DBr 6
#define _trans_DRr 7
#define _trans_RUr 8
#define _trans_RFr 9
#define _trans_RDr 10
#define _trans_RBr 11
#define _trans_LUr 12
#define _trans_LFr 13
#define _trans_LDr 14
#define _trans_LBr 15
#define _trans_FUr 16
#define _trans_FRr 17
#define _trans_FDr 18
#define _trans_FLr 19
#define _trans_BUr 20
#define _trans_BRr 21
#define _trans_BDr 22
#define _trans_BLr 23

#define _trans_UFm 24
#define _trans_ULm 25
#define _trans_UBm 26
#define _trans_URm 27
#define _trans_DFm 28
#define _trans_DLm 29
#define _trans_DBm 30
#define _trans_DRm 31
#define _trans_RUm 32
#define _trans_RFm 33
#define _trans_RDm 34
#define _trans_RBm 35
#define _trans_LUm 36
#define _trans_LFm 37
#define _trans_LDm 38
#define _trans_LBm 39
#define _trans_FUm 40
#define _trans_FRm 41
#define _trans_FDm 42
#define _trans_FLm 43
#define _trans_BUm 44
#define _trans_BRm 45
#define _trans_BDm 46
#define _trans_BLm 47

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
#define _esepbit1   0x4U
#define _esepbit2   0x8U
#define _csepbit    0x4U
#define _eobit      0x10U
#define _cobits     0xF0U
#define _cobits2    0x60U
#define _ctwist_cw  0x20U
#define _ctwist_ccw 0x40U
#define _eflip      0x10U
#define _error      0xFFU

_static cube_t zero = { .corner = {0}, .edge = {0} };
_static cube_t solved = {
	.corner = {0, 1, 2, 3, 4, 5, 6, 7},
	.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
};

#define zero_fast fastcube( \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define solved_fast fastcube( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)

#define _move_cube_U fastcube( \
    5, 4, 2, 3, 0, 1, 6, 7, 4, 5, 2, 3, 1, 0, 6, 7, 8, 9, 10, 11)
#define _move_cube_U2 fastcube( \
    1, 0, 2, 3, 5, 4, 6, 7, 1, 0, 2, 3, 5, 4, 6, 7, 8, 9, 10, 11)
#define _move_cube_U3 fastcube( \
    4, 5, 2, 3, 1, 0, 6, 7, 5, 4, 2, 3, 0, 1, 6, 7, 8, 9, 10, 11)
#define _move_cube_D fastcube( \
    0, 1, 7, 6, 4, 5, 2, 3, 0, 1, 7, 6, 4, 5, 2, 3, 8, 9, 10, 11)
#define _move_cube_D2 fastcube( \
    0, 1, 3, 2, 4, 5, 7, 6, 0, 1, 3, 2, 4, 5, 7, 6, 8, 9, 10, 11)
#define _move_cube_D3 fastcube( \
    0, 1, 6, 7, 4, 5, 3, 2, 0, 1, 6, 7, 4, 5, 3, 2, 8, 9, 10, 11)
#define _move_cube_R fastcube( \
    70, 1, 2, 69, 4, 32, 35, 7, 0, 1, 2, 3, 8, 5, 6, 11, 7, 9, 10, 4)
#define _move_cube_R2 fastcube( \
    3, 1, 2, 0, 4, 6, 5, 7, 0, 1, 2, 3, 7, 5, 6, 4, 11, 9, 10, 8)
#define _move_cube_R3 fastcube( \
    69, 1, 2, 70, 4, 35, 32, 7, 0, 1, 2, 3, 11, 5, 6, 8, 4, 9, 10, 7)
#define _move_cube_L fastcube( \
    0, 71, 68, 3, 33, 5, 6, 34, 0, 1, 2, 3, 4, 10, 9, 7, 8, 5, 6, 11)
#define _move_cube_L2 fastcube( \
    0, 2, 1, 3, 7, 5, 6, 4, 0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11)
#define _move_cube_L3 fastcube( \
    0, 68, 71, 3, 34, 5, 6, 33, 0, 1, 2, 3, 4, 9, 10, 7, 8, 6, 5, 11)
#define _move_cube_F fastcube( \
    36, 1, 38, 3, 66, 5, 64, 7, 25, 1, 2, 24, 4, 5, 6, 7, 16, 19, 10, 11)
#define _move_cube_F2 fastcube( \
    2, 1, 0, 3, 6, 5, 4, 7, 3, 1, 2, 0, 4, 5, 6, 7, 9, 8, 10, 11)
#define _move_cube_F3 fastcube( \
    38, 1, 36, 3, 64, 5, 66, 7, 24, 1, 2, 25, 4, 5, 6, 7, 19, 16, 10, 11)
#define _move_cube_B fastcube( \
    0, 37, 2, 39, 4, 67, 6, 65, 0, 27, 26, 3, 4, 5, 6, 7, 8, 9, 17, 18)
#define _move_cube_B2 fastcube( \
    0, 3, 2, 1, 4, 7, 6, 5, 0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 11, 10)
#define _move_cube_B3 fastcube( \
    0, 39, 2, 37, 4, 65, 6, 67, 0, 26, 27, 3, 4, 5, 6, 7, 8, 9, 18, 17)

#define _trans_cube_UFr fastcube( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)
#define _trans_cube_UFr_inverse fastcube( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)
#define _trans_cube_ULr fastcube( \
    4, 5, 7, 6, 1, 0, 2, 3, 5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24)
#define _trans_cube_ULr_inverse fastcube( \
    5, 4, 6, 7, 0, 1, 3, 2, 4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26)
#define _trans_cube_UBr fastcube( \
    1, 0, 3, 2, 5, 4, 7, 6, 1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9)
#define _trans_cube_UBr_inverse fastcube( \
    1, 0, 3, 2, 5, 4, 7, 6, 1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9)
#define _trans_cube_URr fastcube( \
    5, 4, 6, 7, 0, 1, 3, 2, 4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26)
#define _trans_cube_URr_inverse fastcube( \
    4, 5, 7, 6, 1, 0, 2, 3, 5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24)
#define _trans_cube_DFr fastcube( \
    2, 3, 0, 1, 6, 7, 4, 5, 3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10)
#define _trans_cube_DFr_inverse fastcube( \
    2, 3, 0, 1, 6, 7, 4, 5, 3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10)
#define _trans_cube_DLr fastcube( \
    7, 6, 4, 5, 2, 3, 1, 0, 6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27)
#define _trans_cube_DLr_inverse fastcube( \
    7, 6, 4, 5, 2, 3, 1, 0, 6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27)
#define _trans_cube_DBr fastcube( \
    3, 2, 1, 0, 7, 6, 5, 4, 2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8)
#define _trans_cube_DBr_inverse fastcube( \
    3, 2, 1, 0, 7, 6, 5, 4, 2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8)
#define _trans_cube_DRr fastcube( \
    6, 7, 5, 4, 3, 2, 0, 1, 7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25)
#define _trans_cube_DRr_inverse fastcube( \
    6, 7, 5, 4, 3, 2, 0, 1, 7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25)
#define _trans_cube_RUr fastcube( \
    64, 67, 65, 66, 37, 38, 36, 39, 20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3)
#define _trans_cube_RUr_inverse fastcube( \
    32, 34, 35, 33, 70, 68, 69, 71, 8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21)
#define _trans_cube_RFr fastcube( \
    38, 37, 36, 39, 64, 67, 66, 65, 24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18)
#define _trans_cube_RFr_inverse fastcube( \
    36, 39, 38, 37, 66, 65, 64, 67, 25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17)
#define _trans_cube_RDr fastcube( \
    67, 64, 66, 65, 38, 37, 39, 36, 23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1)
#define _trans_cube_RDr_inverse fastcube( \
    33, 35, 34, 32, 71, 69, 68, 70, 10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20)
#define _trans_cube_RBr fastcube( \
    37, 38, 39, 36, 67, 64, 65, 66, 27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16)
#define _trans_cube_RBr_inverse fastcube( \
    37, 38, 39, 36, 67, 64, 65, 66, 27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16)
#define _trans_cube_LUr fastcube( \
    65, 66, 64, 67, 36, 39, 37, 38, 21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2)
#define _trans_cube_LUr_inverse fastcube( \
    34, 32, 33, 35, 68, 70, 71, 69, 9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23)
#define _trans_cube_LFr fastcube( \
    36, 39, 38, 37, 66, 65, 64, 67, 25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17)
#define _trans_cube_LFr_inverse fastcube( \
    38, 37, 36, 39, 64, 67, 66, 65, 24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18)
#define _trans_cube_LDr fastcube( \
    66, 65, 67, 64, 39, 36, 38, 37, 22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0)
#define _trans_cube_LDr_inverse fastcube( \
    35, 33, 32, 34, 69, 71, 70, 68, 11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22)
#define _trans_cube_LBr fastcube( \
    39, 36, 37, 38, 65, 66, 67, 64, 26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19)
#define _trans_cube_LBr_inverse fastcube( \
    39, 36, 37, 38, 65, 66, 67, 64, 26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19)
#define _trans_cube_FUr fastcube( \
    68, 70, 69, 71, 32, 34, 33, 35, 16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6)
#define _trans_cube_FUr_inverse fastcube( \
    68, 70, 69, 71, 32, 34, 33, 35, 16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6)
#define _trans_cube_FRr fastcube( \
    32, 34, 35, 33, 70, 68, 69, 71, 8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21)
#define _trans_cube_FRr_inverse fastcube( \
    64, 67, 65, 66, 37, 38, 36, 39, 20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3)
#define _trans_cube_FDr fastcube( \
    70, 68, 71, 69, 34, 32, 35, 33, 19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4)
#define _trans_cube_FDr_inverse fastcube( \
    69, 71, 68, 70, 33, 35, 32, 34, 17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7)
#define _trans_cube_FLr fastcube( \
    34, 32, 33, 35, 68, 70, 71, 69, 9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23)
#define _trans_cube_FLr_inverse fastcube( \
    65, 66, 64, 67, 36, 39, 37, 38, 21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2)
#define _trans_cube_BUr fastcube( \
    69, 71, 68, 70, 33, 35, 32, 34, 17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7)
#define _trans_cube_BUr_inverse fastcube( \
    70, 68, 71, 69, 34, 32, 35, 33, 19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4)
#define _trans_cube_BRr fastcube( \
    35, 33, 32, 34, 69, 71, 70, 68, 11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22)
#define _trans_cube_BRr_inverse fastcube( \
    66, 65, 67, 64, 39, 36, 38, 37, 22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0)
#define _trans_cube_BDr fastcube( \
    71, 69, 70, 68, 35, 33, 34, 32, 18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5)
#define _trans_cube_BDr_inverse fastcube( \
    71, 69, 70, 68, 35, 33, 34, 32, 18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5)
#define _trans_cube_BLr fastcube( \
    33, 35, 34, 32, 71, 69, 68, 70, 10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20)
#define _trans_cube_BLr_inverse fastcube( \
    67, 64, 66, 65, 38, 37, 39, 36, 23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1)
#define _trans_cube_UFm fastcube( \
    4, 5, 6, 7, 0, 1, 2, 3, 0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10)
#define _trans_cube_UFm_inverse fastcube( \
    4, 5, 6, 7, 0, 1, 2, 3, 0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10)
#define _trans_cube_ULm fastcube( \
    0, 1, 3, 2, 5, 4, 6, 7, 4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25)
#define _trans_cube_ULm_inverse fastcube( \
    0, 1, 3, 2, 5, 4, 6, 7, 4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25)
#define _trans_cube_UBm fastcube( \
    5, 4, 7, 6, 1, 0, 3, 2, 1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8)
#define _trans_cube_UBm_inverse fastcube( \
    5, 4, 7, 6, 1, 0, 3, 2, 1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8)
#define _trans_cube_URm fastcube( \
    1, 0, 2, 3, 4, 5, 7, 6, 5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27)
#define _trans_cube_URm_inverse fastcube( \
    1, 0, 2, 3, 4, 5, 7, 6, 5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27)
#define _trans_cube_DFm fastcube( \
    6, 7, 4, 5, 2, 3, 0, 1, 3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11)
#define _trans_cube_DFm_inverse fastcube( \
    6, 7, 4, 5, 2, 3, 0, 1, 3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11)
#define _trans_cube_DLm fastcube( \
    3, 2, 0, 1, 6, 7, 5, 4, 7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26)
#define _trans_cube_DLm_inverse fastcube( \
    2, 3, 1, 0, 7, 6, 4, 5, 6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24)
#define _trans_cube_DBm fastcube( \
    7, 6, 5, 4, 3, 2, 1, 0, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9)
#define _trans_cube_DBm_inverse fastcube( \
    7, 6, 5, 4, 3, 2, 1, 0, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9)
#define _trans_cube_DRm fastcube( \
    2, 3, 1, 0, 7, 6, 4, 5, 6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24)
#define _trans_cube_DRm_inverse fastcube( \
    3, 2, 0, 1, 6, 7, 5, 4, 7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26)
#define _trans_cube_RUm fastcube( \
    68, 71, 69, 70, 33, 34, 32, 35, 21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3)
#define _trans_cube_RUm_inverse fastcube( \
    70, 68, 69, 71, 32, 34, 35, 33, 8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22)
#define _trans_cube_RFm fastcube( \
    34, 33, 32, 35, 68, 71, 70, 69, 25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18)
#define _trans_cube_RFm_inverse fastcube( \
    66, 65, 64, 67, 36, 39, 38, 37, 25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18)
#define _trans_cube_RDm fastcube( \
    71, 68, 70, 69, 34, 33, 35, 32, 22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1)
#define _trans_cube_RDm_inverse fastcube( \
    71, 69, 68, 70, 33, 35, 34, 32, 10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23)
#define _trans_cube_RBm fastcube( \
    33, 34, 35, 32, 71, 68, 69, 70, 26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16)
#define _trans_cube_RBm_inverse fastcube( \
    67, 64, 65, 66, 37, 38, 39, 36, 27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19)
#define _trans_cube_LUm fastcube( \
    69, 70, 68, 71, 32, 35, 33, 34, 20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2)
#define _trans_cube_LUm_inverse fastcube( \
    68, 70, 71, 69, 34, 32, 33, 35, 9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20)
#define _trans_cube_LFm fastcube( \
    32, 35, 34, 33, 70, 69, 68, 71, 24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17)
#define _trans_cube_LFm_inverse fastcube( \
    64, 67, 66, 65, 38, 37, 36, 39, 24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17)
#define _trans_cube_LDm fastcube( \
    70, 69, 71, 68, 35, 32, 34, 33, 23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0)
#define _trans_cube_LDm_inverse fastcube( \
    69, 71, 70, 68, 35, 33, 32, 34, 11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21)
#define _trans_cube_LBm fastcube( \
    35, 32, 33, 34, 69, 70, 71, 68, 27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19)
#define _trans_cube_LBm_inverse fastcube( \
    65, 66, 67, 64, 39, 36, 37, 38, 26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16)
#define _trans_cube_FUm fastcube( \
    64, 66, 65, 67, 36, 38, 37, 39, 16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7)
#define _trans_cube_FUm_inverse fastcube( \
    32, 34, 33, 35, 68, 70, 69, 71, 16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7)
#define _trans_cube_FRm fastcube( \
    36, 38, 39, 37, 66, 64, 65, 67, 9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20)
#define _trans_cube_FRm_inverse fastcube( \
    37, 38, 36, 39, 64, 67, 65, 66, 20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2)
#define _trans_cube_FDm fastcube( \
    66, 64, 67, 65, 38, 36, 39, 37, 19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5)
#define _trans_cube_FDm_inverse fastcube( \
    33, 35, 32, 34, 69, 71, 68, 70, 17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6)
#define _trans_cube_FLm fastcube( \
    38, 36, 37, 39, 64, 66, 67, 65, 8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22)
#define _trans_cube_FLm_inverse fastcube( \
    36, 39, 37, 38, 65, 66, 64, 67, 21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3)
#define _trans_cube_BUm fastcube( \
    65, 67, 64, 66, 37, 39, 36, 38, 17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6)
#define _trans_cube_BUm_inverse fastcube( \
    34, 32, 35, 33, 70, 68, 71, 69, 19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5)
#define _trans_cube_BRm fastcube( \
    39, 37, 36, 38, 65, 67, 66, 64, 10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23)
#define _trans_cube_BRm_inverse fastcube( \
    39, 36, 38, 37, 66, 65, 67, 64, 22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1)
#define _trans_cube_BDm fastcube( \
    67, 65, 66, 64, 39, 37, 38, 36, 18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4)
#define _trans_cube_BDm_inverse fastcube( \
    35, 33, 34, 32, 71, 69, 70, 68, 18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4)
#define _trans_cube_BLm fastcube( \
    37, 39, 38, 36, 67, 65, 64, 66, 11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21)
#define _trans_cube_BLm_inverse fastcube( \
    38, 37, 39, 36, 67, 64, 66, 65, 23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0)

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
