#define _move_cube_U static_cube( \
    5, 4, 2, 3, 0, 1, 6, 7, 4, 5, 2, 3, 1, 0, 6, 7, 8, 9, 10, 11)
#define _move_cube_U2 static_cube( \
    1, 0, 2, 3, 5, 4, 6, 7, 1, 0, 2, 3, 5, 4, 6, 7, 8, 9, 10, 11)
#define _move_cube_U3 static_cube( \
    4, 5, 2, 3, 1, 0, 6, 7, 5, 4, 2, 3, 0, 1, 6, 7, 8, 9, 10, 11)
#define _move_cube_D static_cube( \
    0, 1, 7, 6, 4, 5, 2, 3, 0, 1, 7, 6, 4, 5, 2, 3, 8, 9, 10, 11)
#define _move_cube_D2 static_cube( \
    0, 1, 3, 2, 4, 5, 7, 6, 0, 1, 3, 2, 4, 5, 7, 6, 8, 9, 10, 11)
#define _move_cube_D3 static_cube( \
    0, 1, 6, 7, 4, 5, 3, 2, 0, 1, 6, 7, 4, 5, 3, 2, 8, 9, 10, 11)
#define _move_cube_R static_cube( \
    70, 1, 2, 69, 4, 32, 35, 7, 0, 1, 2, 3, 8, 5, 6, 11, 7, 9, 10, 4)
#define _move_cube_R2 static_cube( \
    3, 1, 2, 0, 4, 6, 5, 7, 0, 1, 2, 3, 7, 5, 6, 4, 11, 9, 10, 8)
#define _move_cube_R3 static_cube( \
    69, 1, 2, 70, 4, 35, 32, 7, 0, 1, 2, 3, 11, 5, 6, 8, 4, 9, 10, 7)
#define _move_cube_L static_cube( \
    0, 71, 68, 3, 33, 5, 6, 34, 0, 1, 2, 3, 4, 10, 9, 7, 8, 5, 6, 11)
#define _move_cube_L2 static_cube( \
    0, 2, 1, 3, 7, 5, 6, 4, 0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11)
#define _move_cube_L3 static_cube( \
    0, 68, 71, 3, 34, 5, 6, 33, 0, 1, 2, 3, 4, 9, 10, 7, 8, 6, 5, 11)
#define _move_cube_F static_cube( \
    36, 1, 38, 3, 66, 5, 64, 7, 25, 1, 2, 24, 4, 5, 6, 7, 16, 19, 10, 11)
#define _move_cube_F2 static_cube( \
    2, 1, 0, 3, 6, 5, 4, 7, 3, 1, 2, 0, 4, 5, 6, 7, 9, 8, 10, 11)
#define _move_cube_F3 static_cube( \
    38, 1, 36, 3, 64, 5, 66, 7, 24, 1, 2, 25, 4, 5, 6, 7, 19, 16, 10, 11)
#define _move_cube_B static_cube( \
    0, 37, 2, 39, 4, 67, 6, 65, 0, 27, 26, 3, 4, 5, 6, 7, 8, 9, 17, 18)
#define _move_cube_B2 static_cube( \
    0, 3, 2, 1, 4, 7, 6, 5, 0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 11, 10)
#define _move_cube_B3 static_cube( \
    0, 39, 2, 37, 4, 65, 6, 67, 0, 26, 27, 3, 4, 5, 6, 7, 8, 9, 18, 17)

#define _trans_cube_UFr static_cube( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)
#define _trans_cube_UFr_inverse static_cube( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)
#define _trans_cube_ULr static_cube( \
    4, 5, 7, 6, 1, 0, 2, 3, 5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24)
#define _trans_cube_ULr_inverse static_cube( \
    5, 4, 6, 7, 0, 1, 3, 2, 4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26)
#define _trans_cube_UBr static_cube( \
    1, 0, 3, 2, 5, 4, 7, 6, 1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9)
#define _trans_cube_UBr_inverse static_cube( \
    1, 0, 3, 2, 5, 4, 7, 6, 1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9)
#define _trans_cube_URr static_cube( \
    5, 4, 6, 7, 0, 1, 3, 2, 4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26)
#define _trans_cube_URr_inverse static_cube( \
    4, 5, 7, 6, 1, 0, 2, 3, 5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24)
#define _trans_cube_DFr static_cube( \
    2, 3, 0, 1, 6, 7, 4, 5, 3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10)
#define _trans_cube_DFr_inverse static_cube( \
    2, 3, 0, 1, 6, 7, 4, 5, 3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10)
#define _trans_cube_DLr static_cube( \
    7, 6, 4, 5, 2, 3, 1, 0, 6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27)
#define _trans_cube_DLr_inverse static_cube( \
    7, 6, 4, 5, 2, 3, 1, 0, 6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27)
#define _trans_cube_DBr static_cube( \
    3, 2, 1, 0, 7, 6, 5, 4, 2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8)
#define _trans_cube_DBr_inverse static_cube( \
    3, 2, 1, 0, 7, 6, 5, 4, 2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8)
#define _trans_cube_DRr static_cube( \
    6, 7, 5, 4, 3, 2, 0, 1, 7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25)
#define _trans_cube_DRr_inverse static_cube( \
    6, 7, 5, 4, 3, 2, 0, 1, 7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25)
#define _trans_cube_RUr static_cube( \
    64, 67, 65, 66, 37, 38, 36, 39, 20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3)
#define _trans_cube_RUr_inverse static_cube( \
    32, 34, 35, 33, 70, 68, 69, 71, 8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21)
#define _trans_cube_RFr static_cube( \
    38, 37, 36, 39, 64, 67, 66, 65, 24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18)
#define _trans_cube_RFr_inverse static_cube( \
    36, 39, 38, 37, 66, 65, 64, 67, 25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17)
#define _trans_cube_RDr static_cube( \
    67, 64, 66, 65, 38, 37, 39, 36, 23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1)
#define _trans_cube_RDr_inverse static_cube( \
    33, 35, 34, 32, 71, 69, 68, 70, 10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20)
#define _trans_cube_RBr static_cube( \
    37, 38, 39, 36, 67, 64, 65, 66, 27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16)
#define _trans_cube_RBr_inverse static_cube( \
    37, 38, 39, 36, 67, 64, 65, 66, 27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16)
#define _trans_cube_LUr static_cube( \
    65, 66, 64, 67, 36, 39, 37, 38, 21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2)
#define _trans_cube_LUr_inverse static_cube( \
    34, 32, 33, 35, 68, 70, 71, 69, 9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23)
#define _trans_cube_LFr static_cube( \
    36, 39, 38, 37, 66, 65, 64, 67, 25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17)
#define _trans_cube_LFr_inverse static_cube( \
    38, 37, 36, 39, 64, 67, 66, 65, 24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18)
#define _trans_cube_LDr static_cube( \
    66, 65, 67, 64, 39, 36, 38, 37, 22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0)
#define _trans_cube_LDr_inverse static_cube( \
    35, 33, 32, 34, 69, 71, 70, 68, 11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22)
#define _trans_cube_LBr static_cube( \
    39, 36, 37, 38, 65, 66, 67, 64, 26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19)
#define _trans_cube_LBr_inverse static_cube( \
    39, 36, 37, 38, 65, 66, 67, 64, 26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19)
#define _trans_cube_FUr static_cube( \
    68, 70, 69, 71, 32, 34, 33, 35, 16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6)
#define _trans_cube_FUr_inverse static_cube( \
    68, 70, 69, 71, 32, 34, 33, 35, 16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6)
#define _trans_cube_FRr static_cube( \
    32, 34, 35, 33, 70, 68, 69, 71, 8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21)
#define _trans_cube_FRr_inverse static_cube( \
    64, 67, 65, 66, 37, 38, 36, 39, 20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3)
#define _trans_cube_FDr static_cube( \
    70, 68, 71, 69, 34, 32, 35, 33, 19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4)
#define _trans_cube_FDr_inverse static_cube( \
    69, 71, 68, 70, 33, 35, 32, 34, 17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7)
#define _trans_cube_FLr static_cube( \
    34, 32, 33, 35, 68, 70, 71, 69, 9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23)
#define _trans_cube_FLr_inverse static_cube( \
    65, 66, 64, 67, 36, 39, 37, 38, 21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2)
#define _trans_cube_BUr static_cube( \
    69, 71, 68, 70, 33, 35, 32, 34, 17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7)
#define _trans_cube_BUr_inverse static_cube( \
    70, 68, 71, 69, 34, 32, 35, 33, 19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4)
#define _trans_cube_BRr static_cube( \
    35, 33, 32, 34, 69, 71, 70, 68, 11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22)
#define _trans_cube_BRr_inverse static_cube( \
    66, 65, 67, 64, 39, 36, 38, 37, 22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0)
#define _trans_cube_BDr static_cube( \
    71, 69, 70, 68, 35, 33, 34, 32, 18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5)
#define _trans_cube_BDr_inverse static_cube( \
    71, 69, 70, 68, 35, 33, 34, 32, 18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5)
#define _trans_cube_BLr static_cube( \
    33, 35, 34, 32, 71, 69, 68, 70, 10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20)
#define _trans_cube_BLr_inverse static_cube( \
    67, 64, 66, 65, 38, 37, 39, 36, 23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1)
#define _trans_cube_UFm static_cube( \
    4, 5, 6, 7, 0, 1, 2, 3, 0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10)
#define _trans_cube_UFm_inverse static_cube( \
    4, 5, 6, 7, 0, 1, 2, 3, 0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10)
#define _trans_cube_ULm static_cube( \
    0, 1, 3, 2, 5, 4, 6, 7, 4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25)
#define _trans_cube_ULm_inverse static_cube( \
    0, 1, 3, 2, 5, 4, 6, 7, 4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25)
#define _trans_cube_UBm static_cube( \
    5, 4, 7, 6, 1, 0, 3, 2, 1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8)
#define _trans_cube_UBm_inverse static_cube( \
    5, 4, 7, 6, 1, 0, 3, 2, 1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8)
#define _trans_cube_URm static_cube( \
    1, 0, 2, 3, 4, 5, 7, 6, 5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27)
#define _trans_cube_URm_inverse static_cube( \
    1, 0, 2, 3, 4, 5, 7, 6, 5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27)
#define _trans_cube_DFm static_cube( \
    6, 7, 4, 5, 2, 3, 0, 1, 3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11)
#define _trans_cube_DFm_inverse static_cube( \
    6, 7, 4, 5, 2, 3, 0, 1, 3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11)
#define _trans_cube_DLm static_cube( \
    3, 2, 0, 1, 6, 7, 5, 4, 7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26)
#define _trans_cube_DLm_inverse static_cube( \
    2, 3, 1, 0, 7, 6, 4, 5, 6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24)
#define _trans_cube_DBm static_cube( \
    7, 6, 5, 4, 3, 2, 1, 0, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9)
#define _trans_cube_DBm_inverse static_cube( \
    7, 6, 5, 4, 3, 2, 1, 0, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9)
#define _trans_cube_DRm static_cube( \
    2, 3, 1, 0, 7, 6, 4, 5, 6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24)
#define _trans_cube_DRm_inverse static_cube( \
    3, 2, 0, 1, 6, 7, 5, 4, 7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26)
#define _trans_cube_RUm static_cube( \
    68, 71, 69, 70, 33, 34, 32, 35, 21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3)
#define _trans_cube_RUm_inverse static_cube( \
    70, 68, 69, 71, 32, 34, 35, 33, 8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22)
#define _trans_cube_RFm static_cube( \
    34, 33, 32, 35, 68, 71, 70, 69, 25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18)
#define _trans_cube_RFm_inverse static_cube( \
    66, 65, 64, 67, 36, 39, 38, 37, 25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18)
#define _trans_cube_RDm static_cube( \
    71, 68, 70, 69, 34, 33, 35, 32, 22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1)
#define _trans_cube_RDm_inverse static_cube( \
    71, 69, 68, 70, 33, 35, 34, 32, 10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23)
#define _trans_cube_RBm static_cube( \
    33, 34, 35, 32, 71, 68, 69, 70, 26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16)
#define _trans_cube_RBm_inverse static_cube( \
    67, 64, 65, 66, 37, 38, 39, 36, 27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19)
#define _trans_cube_LUm static_cube( \
    69, 70, 68, 71, 32, 35, 33, 34, 20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2)
#define _trans_cube_LUm_inverse static_cube( \
    68, 70, 71, 69, 34, 32, 33, 35, 9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20)
#define _trans_cube_LFm static_cube( \
    32, 35, 34, 33, 70, 69, 68, 71, 24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17)
#define _trans_cube_LFm_inverse static_cube( \
    64, 67, 66, 65, 38, 37, 36, 39, 24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17)
#define _trans_cube_LDm static_cube( \
    70, 69, 71, 68, 35, 32, 34, 33, 23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0)
#define _trans_cube_LDm_inverse static_cube( \
    69, 71, 70, 68, 35, 33, 32, 34, 11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21)
#define _trans_cube_LBm static_cube( \
    35, 32, 33, 34, 69, 70, 71, 68, 27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19)
#define _trans_cube_LBm_inverse static_cube( \
    65, 66, 67, 64, 39, 36, 37, 38, 26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16)
#define _trans_cube_FUm static_cube( \
    64, 66, 65, 67, 36, 38, 37, 39, 16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7)
#define _trans_cube_FUm_inverse static_cube( \
    32, 34, 33, 35, 68, 70, 69, 71, 16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7)
#define _trans_cube_FRm static_cube( \
    36, 38, 39, 37, 66, 64, 65, 67, 9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20)
#define _trans_cube_FRm_inverse static_cube( \
    37, 38, 36, 39, 64, 67, 65, 66, 20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2)
#define _trans_cube_FDm static_cube( \
    66, 64, 67, 65, 38, 36, 39, 37, 19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5)
#define _trans_cube_FDm_inverse static_cube( \
    33, 35, 32, 34, 69, 71, 68, 70, 17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6)
#define _trans_cube_FLm static_cube( \
    38, 36, 37, 39, 64, 66, 67, 65, 8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22)
#define _trans_cube_FLm_inverse static_cube( \
    36, 39, 37, 38, 65, 66, 64, 67, 21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3)
#define _trans_cube_BUm static_cube( \
    65, 67, 64, 66, 37, 39, 36, 38, 17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6)
#define _trans_cube_BUm_inverse static_cube( \
    34, 32, 35, 33, 70, 68, 71, 69, 19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5)
#define _trans_cube_BRm static_cube( \
    39, 37, 36, 38, 65, 67, 66, 64, 10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23)
#define _trans_cube_BRm_inverse static_cube( \
    39, 36, 38, 37, 66, 65, 67, 64, 22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1)
#define _trans_cube_BDm static_cube( \
    67, 65, 66, 64, 39, 37, 38, 36, 18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4)
#define _trans_cube_BDm_inverse static_cube( \
    35, 33, 34, 32, 71, 69, 70, 68, 18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4)
#define _trans_cube_BLm static_cube( \
    37, 39, 38, 36, 67, 65, 64, 66, 11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21)
#define _trans_cube_BLm_inverse static_cube( \
    38, 37, 39, 36, 67, 64, 66, 65, 23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0)
