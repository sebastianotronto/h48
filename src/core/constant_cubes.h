#define MOVE_CUBE_U STATIC_CUBE( \
    5, 4, 2, 3, 0, 1, 6, 7, 4, 5, 2, 3, 1, 0, 6, 7, 8, 9, 10, 11)
#define MOVE_CUBE_U2 STATIC_CUBE( \
    1, 0, 2, 3, 5, 4, 6, 7, 1, 0, 2, 3, 5, 4, 6, 7, 8, 9, 10, 11)
#define MOVE_CUBE_U3 STATIC_CUBE( \
    4, 5, 2, 3, 1, 0, 6, 7, 5, 4, 2, 3, 0, 1, 6, 7, 8, 9, 10, 11)
#define MOVE_CUBE_D STATIC_CUBE( \
    0, 1, 7, 6, 4, 5, 2, 3, 0, 1, 7, 6, 4, 5, 2, 3, 8, 9, 10, 11)
#define MOVE_CUBE_D2 STATIC_CUBE( \
    0, 1, 3, 2, 4, 5, 7, 6, 0, 1, 3, 2, 4, 5, 7, 6, 8, 9, 10, 11)
#define MOVE_CUBE_D3 STATIC_CUBE( \
    0, 1, 6, 7, 4, 5, 3, 2, 0, 1, 6, 7, 4, 5, 3, 2, 8, 9, 10, 11)
#define MOVE_CUBE_R STATIC_CUBE( \
    70, 1, 2, 69, 4, 32, 35, 7, 0, 1, 2, 3, 8, 5, 6, 11, 7, 9, 10, 4)
#define MOVE_CUBE_R2 STATIC_CUBE( \
    3, 1, 2, 0, 4, 6, 5, 7, 0, 1, 2, 3, 7, 5, 6, 4, 11, 9, 10, 8)
#define MOVE_CUBE_R3 STATIC_CUBE( \
    69, 1, 2, 70, 4, 35, 32, 7, 0, 1, 2, 3, 11, 5, 6, 8, 4, 9, 10, 7)
#define MOVE_CUBE_L STATIC_CUBE( \
    0, 71, 68, 3, 33, 5, 6, 34, 0, 1, 2, 3, 4, 10, 9, 7, 8, 5, 6, 11)
#define MOVE_CUBE_L2 STATIC_CUBE( \
    0, 2, 1, 3, 7, 5, 6, 4, 0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11)
#define MOVE_CUBE_L3 STATIC_CUBE( \
    0, 68, 71, 3, 34, 5, 6, 33, 0, 1, 2, 3, 4, 9, 10, 7, 8, 6, 5, 11)
#define MOVE_CUBE_F STATIC_CUBE( \
    36, 1, 38, 3, 66, 5, 64, 7, 25, 1, 2, 24, 4, 5, 6, 7, 16, 19, 10, 11)
#define MOVE_CUBE_F2 STATIC_CUBE( \
    2, 1, 0, 3, 6, 5, 4, 7, 3, 1, 2, 0, 4, 5, 6, 7, 9, 8, 10, 11)
#define MOVE_CUBE_F3 STATIC_CUBE( \
    38, 1, 36, 3, 64, 5, 66, 7, 24, 1, 2, 25, 4, 5, 6, 7, 19, 16, 10, 11)
#define MOVE_CUBE_B STATIC_CUBE( \
    0, 37, 2, 39, 4, 67, 6, 65, 0, 27, 26, 3, 4, 5, 6, 7, 8, 9, 17, 18)
#define MOVE_CUBE_B2 STATIC_CUBE( \
    0, 3, 2, 1, 4, 7, 6, 5, 0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 11, 10)
#define MOVE_CUBE_B3 STATIC_CUBE( \
    0, 39, 2, 37, 4, 65, 6, 67, 0, 26, 27, 3, 4, 5, 6, 7, 8, 9, 18, 17)

#define TRANS_CUBE_UFr STATIC_CUBE( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)
#define TRANS_CUBE_UFr_INVERSE STATIC_CUBE( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)
#define TRANS_CUBE_ULr STATIC_CUBE( \
    4, 5, 7, 6, 1, 0, 2, 3, 5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24)
#define TRANS_CUBE_ULr_INVERSE STATIC_CUBE( \
    5, 4, 6, 7, 0, 1, 3, 2, 4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26)
#define TRANS_CUBE_UBr STATIC_CUBE( \
    1, 0, 3, 2, 5, 4, 7, 6, 1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9)
#define TRANS_CUBE_UBr_INVERSE STATIC_CUBE( \
    1, 0, 3, 2, 5, 4, 7, 6, 1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9)
#define TRANS_CUBE_URr STATIC_CUBE( \
    5, 4, 6, 7, 0, 1, 3, 2, 4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26)
#define TRANS_CUBE_URr_INVERSE STATIC_CUBE( \
    4, 5, 7, 6, 1, 0, 2, 3, 5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24)
#define TRANS_CUBE_DFr STATIC_CUBE( \
    2, 3, 0, 1, 6, 7, 4, 5, 3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10)
#define TRANS_CUBE_DFr_INVERSE STATIC_CUBE( \
    2, 3, 0, 1, 6, 7, 4, 5, 3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10)
#define TRANS_CUBE_DLr STATIC_CUBE( \
    7, 6, 4, 5, 2, 3, 1, 0, 6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27)
#define TRANS_CUBE_DLr_INVERSE STATIC_CUBE( \
    7, 6, 4, 5, 2, 3, 1, 0, 6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27)
#define TRANS_CUBE_DBr STATIC_CUBE( \
    3, 2, 1, 0, 7, 6, 5, 4, 2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8)
#define TRANS_CUBE_DBr_INVERSE STATIC_CUBE( \
    3, 2, 1, 0, 7, 6, 5, 4, 2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8)
#define TRANS_CUBE_DRr STATIC_CUBE( \
    6, 7, 5, 4, 3, 2, 0, 1, 7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25)
#define TRANS_CUBE_DRr_INVERSE STATIC_CUBE( \
    6, 7, 5, 4, 3, 2, 0, 1, 7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25)
#define TRANS_CUBE_RUr STATIC_CUBE( \
    64, 67, 65, 66, 37, 38, 36, 39, 20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3)
#define TRANS_CUBE_RUr_INVERSE STATIC_CUBE( \
    32, 34, 35, 33, 70, 68, 69, 71, 8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21)
#define TRANS_CUBE_RFr STATIC_CUBE( \
    38, 37, 36, 39, 64, 67, 66, 65, 24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18)
#define TRANS_CUBE_RFr_INVERSE STATIC_CUBE( \
    36, 39, 38, 37, 66, 65, 64, 67, 25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17)
#define TRANS_CUBE_RDr STATIC_CUBE( \
    67, 64, 66, 65, 38, 37, 39, 36, 23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1)
#define TRANS_CUBE_RDr_INVERSE STATIC_CUBE( \
    33, 35, 34, 32, 71, 69, 68, 70, 10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20)
#define TRANS_CUBE_RBr STATIC_CUBE( \
    37, 38, 39, 36, 67, 64, 65, 66, 27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16)
#define TRANS_CUBE_RBr_INVERSE STATIC_CUBE( \
    37, 38, 39, 36, 67, 64, 65, 66, 27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16)
#define TRANS_CUBE_LUr STATIC_CUBE( \
    65, 66, 64, 67, 36, 39, 37, 38, 21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2)
#define TRANS_CUBE_LUr_INVERSE STATIC_CUBE( \
    34, 32, 33, 35, 68, 70, 71, 69, 9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23)
#define TRANS_CUBE_LFr STATIC_CUBE( \
    36, 39, 38, 37, 66, 65, 64, 67, 25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17)
#define TRANS_CUBE_LFr_INVERSE STATIC_CUBE( \
    38, 37, 36, 39, 64, 67, 66, 65, 24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18)
#define TRANS_CUBE_LDr STATIC_CUBE( \
    66, 65, 67, 64, 39, 36, 38, 37, 22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0)
#define TRANS_CUBE_LDr_INVERSE STATIC_CUBE( \
    35, 33, 32, 34, 69, 71, 70, 68, 11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22)
#define TRANS_CUBE_LBr STATIC_CUBE( \
    39, 36, 37, 38, 65, 66, 67, 64, 26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19)
#define TRANS_CUBE_LBr_INVERSE STATIC_CUBE( \
    39, 36, 37, 38, 65, 66, 67, 64, 26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19)
#define TRANS_CUBE_FUr STATIC_CUBE( \
    68, 70, 69, 71, 32, 34, 33, 35, 16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6)
#define TRANS_CUBE_FUr_INVERSE STATIC_CUBE( \
    68, 70, 69, 71, 32, 34, 33, 35, 16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6)
#define TRANS_CUBE_FRr STATIC_CUBE( \
    32, 34, 35, 33, 70, 68, 69, 71, 8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21)
#define TRANS_CUBE_FRr_INVERSE STATIC_CUBE( \
    64, 67, 65, 66, 37, 38, 36, 39, 20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3)
#define TRANS_CUBE_FDr STATIC_CUBE( \
    70, 68, 71, 69, 34, 32, 35, 33, 19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4)
#define TRANS_CUBE_FDr_INVERSE STATIC_CUBE( \
    69, 71, 68, 70, 33, 35, 32, 34, 17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7)
#define TRANS_CUBE_FLr STATIC_CUBE( \
    34, 32, 33, 35, 68, 70, 71, 69, 9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23)
#define TRANS_CUBE_FLr_INVERSE STATIC_CUBE( \
    65, 66, 64, 67, 36, 39, 37, 38, 21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2)
#define TRANS_CUBE_BUr STATIC_CUBE( \
    69, 71, 68, 70, 33, 35, 32, 34, 17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7)
#define TRANS_CUBE_BUr_INVERSE STATIC_CUBE( \
    70, 68, 71, 69, 34, 32, 35, 33, 19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4)
#define TRANS_CUBE_BRr STATIC_CUBE( \
    35, 33, 32, 34, 69, 71, 70, 68, 11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22)
#define TRANS_CUBE_BRr_INVERSE STATIC_CUBE( \
    66, 65, 67, 64, 39, 36, 38, 37, 22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0)
#define TRANS_CUBE_BDr STATIC_CUBE( \
    71, 69, 70, 68, 35, 33, 34, 32, 18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5)
#define TRANS_CUBE_BDr_INVERSE STATIC_CUBE( \
    71, 69, 70, 68, 35, 33, 34, 32, 18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5)
#define TRANS_CUBE_BLr STATIC_CUBE( \
    33, 35, 34, 32, 71, 69, 68, 70, 10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20)
#define TRANS_CUBE_BLr_INVERSE STATIC_CUBE( \
    67, 64, 66, 65, 38, 37, 39, 36, 23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1)
#define TRANS_CUBE_UFm STATIC_CUBE( \
    4, 5, 6, 7, 0, 1, 2, 3, 0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10)
#define TRANS_CUBE_UFm_INVERSE STATIC_CUBE( \
    4, 5, 6, 7, 0, 1, 2, 3, 0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10)
#define TRANS_CUBE_ULm STATIC_CUBE( \
    0, 1, 3, 2, 5, 4, 6, 7, 4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25)
#define TRANS_CUBE_ULm_INVERSE STATIC_CUBE( \
    0, 1, 3, 2, 5, 4, 6, 7, 4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25)
#define TRANS_CUBE_UBm STATIC_CUBE( \
    5, 4, 7, 6, 1, 0, 3, 2, 1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8)
#define TRANS_CUBE_UBm_INVERSE STATIC_CUBE( \
    5, 4, 7, 6, 1, 0, 3, 2, 1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8)
#define TRANS_CUBE_URm STATIC_CUBE( \
    1, 0, 2, 3, 4, 5, 7, 6, 5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27)
#define TRANS_CUBE_URm_INVERSE STATIC_CUBE( \
    1, 0, 2, 3, 4, 5, 7, 6, 5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27)
#define TRANS_CUBE_DFm STATIC_CUBE( \
    6, 7, 4, 5, 2, 3, 0, 1, 3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11)
#define TRANS_CUBE_DFm_INVERSE STATIC_CUBE( \
    6, 7, 4, 5, 2, 3, 0, 1, 3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11)
#define TRANS_CUBE_DLm STATIC_CUBE( \
    3, 2, 0, 1, 6, 7, 5, 4, 7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26)
#define TRANS_CUBE_DLm_INVERSE STATIC_CUBE( \
    2, 3, 1, 0, 7, 6, 4, 5, 6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24)
#define TRANS_CUBE_DBm STATIC_CUBE( \
    7, 6, 5, 4, 3, 2, 1, 0, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9)
#define TRANS_CUBE_DBm_INVERSE STATIC_CUBE( \
    7, 6, 5, 4, 3, 2, 1, 0, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9)
#define TRANS_CUBE_DRm STATIC_CUBE( \
    2, 3, 1, 0, 7, 6, 4, 5, 6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24)
#define TRANS_CUBE_DRm_INVERSE STATIC_CUBE( \
    3, 2, 0, 1, 6, 7, 5, 4, 7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26)
#define TRANS_CUBE_RUm STATIC_CUBE( \
    68, 71, 69, 70, 33, 34, 32, 35, 21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3)
#define TRANS_CUBE_RUm_INVERSE STATIC_CUBE( \
    70, 68, 69, 71, 32, 34, 35, 33, 8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22)
#define TRANS_CUBE_RFm STATIC_CUBE( \
    34, 33, 32, 35, 68, 71, 70, 69, 25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18)
#define TRANS_CUBE_RFm_INVERSE STATIC_CUBE( \
    66, 65, 64, 67, 36, 39, 38, 37, 25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18)
#define TRANS_CUBE_RDm STATIC_CUBE( \
    71, 68, 70, 69, 34, 33, 35, 32, 22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1)
#define TRANS_CUBE_RDm_INVERSE STATIC_CUBE( \
    71, 69, 68, 70, 33, 35, 34, 32, 10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23)
#define TRANS_CUBE_RBm STATIC_CUBE( \
    33, 34, 35, 32, 71, 68, 69, 70, 26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16)
#define TRANS_CUBE_RBm_INVERSE STATIC_CUBE( \
    67, 64, 65, 66, 37, 38, 39, 36, 27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19)
#define TRANS_CUBE_LUm STATIC_CUBE( \
    69, 70, 68, 71, 32, 35, 33, 34, 20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2)
#define TRANS_CUBE_LUm_INVERSE STATIC_CUBE( \
    68, 70, 71, 69, 34, 32, 33, 35, 9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20)
#define TRANS_CUBE_LFm STATIC_CUBE( \
    32, 35, 34, 33, 70, 69, 68, 71, 24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17)
#define TRANS_CUBE_LFm_INVERSE STATIC_CUBE( \
    64, 67, 66, 65, 38, 37, 36, 39, 24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17)
#define TRANS_CUBE_LDm STATIC_CUBE( \
    70, 69, 71, 68, 35, 32, 34, 33, 23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0)
#define TRANS_CUBE_LDm_INVERSE STATIC_CUBE( \
    69, 71, 70, 68, 35, 33, 32, 34, 11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21)
#define TRANS_CUBE_LBm STATIC_CUBE( \
    35, 32, 33, 34, 69, 70, 71, 68, 27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19)
#define TRANS_CUBE_LBm_INVERSE STATIC_CUBE( \
    65, 66, 67, 64, 39, 36, 37, 38, 26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16)
#define TRANS_CUBE_FUm STATIC_CUBE( \
    64, 66, 65, 67, 36, 38, 37, 39, 16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7)
#define TRANS_CUBE_FUm_INVERSE STATIC_CUBE( \
    32, 34, 33, 35, 68, 70, 69, 71, 16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7)
#define TRANS_CUBE_FRm STATIC_CUBE( \
    36, 38, 39, 37, 66, 64, 65, 67, 9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20)
#define TRANS_CUBE_FRm_INVERSE STATIC_CUBE( \
    37, 38, 36, 39, 64, 67, 65, 66, 20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2)
#define TRANS_CUBE_FDm STATIC_CUBE( \
    66, 64, 67, 65, 38, 36, 39, 37, 19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5)
#define TRANS_CUBE_FDm_INVERSE STATIC_CUBE( \
    33, 35, 32, 34, 69, 71, 68, 70, 17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6)
#define TRANS_CUBE_FLm STATIC_CUBE( \
    38, 36, 37, 39, 64, 66, 67, 65, 8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22)
#define TRANS_CUBE_FLm_INVERSE STATIC_CUBE( \
    36, 39, 37, 38, 65, 66, 64, 67, 21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3)
#define TRANS_CUBE_BUm STATIC_CUBE( \
    65, 67, 64, 66, 37, 39, 36, 38, 17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6)
#define TRANS_CUBE_BUm_INVERSE STATIC_CUBE( \
    34, 32, 35, 33, 70, 68, 71, 69, 19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5)
#define TRANS_CUBE_BRm STATIC_CUBE( \
    39, 37, 36, 38, 65, 67, 66, 64, 10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23)
#define TRANS_CUBE_BRm_INVERSE STATIC_CUBE( \
    39, 36, 38, 37, 66, 65, 67, 64, 22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1)
#define TRANS_CUBE_BDm STATIC_CUBE( \
    67, 65, 66, 64, 39, 37, 38, 36, 18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4)
#define TRANS_CUBE_BDm_INVERSE STATIC_CUBE( \
    35, 33, 34, 32, 71, 69, 70, 68, 18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4)
#define TRANS_CUBE_BLm STATIC_CUBE( \
    37, 39, 38, 36, 67, 65, 64, 66, 11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21)
#define TRANS_CUBE_BLm_INVERSE STATIC_CUBE( \
    38, 37, 39, 36, 67, 64, 66, 65, 23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0)
