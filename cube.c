#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifdef CUBE_AVX2
#include <immintrin.h>
#endif

#ifdef DEBUG
#include <stdio.h>
#define _static
#define _static_inline
#define DBG_LOG(...) fprintf(stderr, __VA_ARGS__)
#define DBG_WARN(condition, ...) if (!(condition)) DBG_LOG(__VA_ARGS__);
#define DBG_ASSERT(condition, retval, ...) \
	if (!(condition)) {                \
		DBG_LOG(__VA_ARGS__);      \
		return retval;             \
	}
#else
#define _static static
#define _static_inline static inline
#define DBG_LOG(...)
#define DBG_WARN(condition, ...)
#define DBG_ASSERT(condition, retval, ...)
#endif

#include "cube.h"

/******************************************************************************
Section: constants, strings and other stuff
******************************************************************************/

#define U  0U
#define U2 1U
#define U3 2U
#define D  3U
#define D2 4U
#define D3 5U
#define R  6U
#define R2 7U
#define R3 8U
#define L  9U
#define L2 10U
#define L3 11U
#define F  12U
#define F2 13U
#define F3 14U
#define B  15U
#define B2 16U
#define B3 17U

#define UFr 0
#define ULr 1
#define UBr 2
#define URr 3
#define DFr 4
#define DLr 5
#define DBr 6
#define DRr 7
#define RUr 8
#define RFr 9
#define RDr 10
#define RBr 11
#define LUr 12
#define LFr 13
#define LDr 14
#define LBr 15
#define FUr 16
#define FRr 17
#define FDr 18
#define FLr 19
#define BUr 20
#define BRr 21
#define BDr 22
#define BLr 23

#define UFm 24
#define ULm 25
#define UBm 26
#define URm 27
#define DFm 28
#define DLm 29
#define DBm 30
#define DRm 31
#define RUm 32
#define RFm 33
#define RDm 34
#define RBm 35
#define LUm 36
#define LFm 37
#define LDm 38
#define LBm 39
#define FUm 40
#define FRm 41
#define FDm 42
#define FLm 43
#define BUm 44
#define BRm 45
#define BDm 46
#define BLm 47

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

/******************************************************************************
Section: AVX2 fast methods

This section contains performance-critical methods that rely on AVX2
intructions such as routines for moving or transforming the cube.

Note: the #ifdef below is closed in the next section.
******************************************************************************/

#ifdef CUBE_AVX2

typedef __m256i cube_fast_t;

#define _co2_avx2 _mm256_set_epi64x(0, 0, 0, 0x6060606060606060)
#define _cocw_avx2 _mm256_set_epi64x(0, 0, 0, 0x2020202020202020)
#define _eo_avx2 _mm256_set_epi64x(0x10101010, 0x1010101010101010, 0, 0)
#define zero_fast _mm256_set_epi64x(0, 0, 0, 0);
#define solved_fast _mm256_set_epi8(                      \
	0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 5, 4, 3, 2, 1, 0    \
)

#define _move_cube_U _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 0, 1, 3, 2, 5, 4, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 1, 0, 3, 2, 4, 5)
#define _move_cube_U2 _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 4, 5, 3, 2, 0, 1, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 3, 2, 0, 1)
#define _move_cube_U3 _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 1, 0, 3, 2, 4, 5, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 0, 1, 3, 2, 5, 4)
#define _move_cube_D _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 3, 2, 5, 4, 6, 7, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 5, 4, 6, 7, 1, 0)
#define _move_cube_D2 _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 6, 7, 5, 4, 2, 3, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 2, 3, 1, 0)
#define _move_cube_D3 _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 2, 3, 5, 4, 7, 6, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 5, 4, 7, 6, 1, 0)
#define _move_cube_R _mm256_set_epi8( \
	0, 0, 0, 0, 4, 10, 9, 7, 11, 6, 5, 8, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 35, 32, 4, 69, 2, 1, 70)
#define _move_cube_R2 _mm256_set_epi8( \
	0, 0, 0, 0, 8, 10, 9, 11, 4, 6, 5, 7, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 5, 6, 4, 0, 2, 1, 3)
#define _move_cube_R3 _mm256_set_epi8( \
	0, 0, 0, 0, 7, 10, 9, 4, 8, 6, 5, 11, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 32, 35, 4, 70, 2, 1, 69)
#define _move_cube_L _mm256_set_epi8( \
	0, 0, 0, 0, 11, 6, 5, 8, 7, 9, 10, 4, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 34, 6, 5, 33, 3, 68, 71, 0)
#define _move_cube_L2 _mm256_set_epi8( \
	0, 0, 0, 0, 11, 9, 10, 8, 7, 5, 6, 4, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 5, 7, 3, 1, 2, 0)
#define _move_cube_L3 _mm256_set_epi8( \
	0, 0, 0, 0, 11, 5, 6, 8, 7, 10, 9, 4, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 33, 6, 5, 34, 3, 71, 68, 0)
#define _move_cube_F _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 19, 16, 7, 6, 5, 4, 24, 2, 1, 25, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 64, 5, 66, 3, 38, 1, 36)
#define _move_cube_F2 _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 8, 9, 7, 6, 5, 4, 0, 2, 1, 3, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 4, 5, 6, 3, 0, 1, 2)
#define _move_cube_F3 _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 16, 19, 7, 6, 5, 4, 25, 2, 1, 24, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 66, 5, 64, 3, 36, 1, 38)
#define _move_cube_B _mm256_set_epi8( \
	0, 0, 0, 0, 18, 17, 9, 8, 7, 6, 5, 4, 3, 26, 27, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 65, 6, 67, 4, 39, 2, 37, 0)
#define _move_cube_B2 _mm256_set_epi8( \
	0, 0, 0, 0, 10, 11, 9, 8, 7, 6, 5, 4, 3, 1, 2, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 7, 4, 1, 2, 3, 0)
#define _move_cube_B3 _mm256_set_epi8( \
	0, 0, 0, 0, 17, 18, 9, 8, 7, 6, 5, 4, 3, 27, 26, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 67, 6, 65, 4, 37, 2, 39, 0)

#define _trans_cube_UFr _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 5, 4, 3, 2, 1, 0)
#define _trans_cube_UFr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 5, 4, 3, 2, 1, 0)
#define _trans_cube_ULr _mm256_set_epi8( \
	0, 0, 0, 0, 24, 27, 26, 25, 3, 2, 1, 0, 6, 7, 4, 5, \
	0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 0, 1, 6, 7, 5, 4)
#define _trans_cube_ULr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 26, 25, 24, 27, 2, 3, 0, 1, 7, 6, 5, 4, \
	0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0, 7, 6, 4, 5)
#define _trans_cube_UBr _mm256_set_epi8( \
	0, 0, 0, 0, 9, 8, 11, 10, 6, 7, 4, 5, 2, 3, 0, 1, \
	0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 4, 5, 2, 3, 0, 1)
#define _trans_cube_UBr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 9, 8, 11, 10, 6, 7, 4, 5, 2, 3, 0, 1, \
	0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 4, 5, 2, 3, 0, 1)
#define _trans_cube_URr _mm256_set_epi8( \
	0, 0, 0, 0, 26, 25, 24, 27, 2, 3, 0, 1, 7, 6, 5, 4, \
	0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0, 7, 6, 4, 5)
#define _trans_cube_URr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 24, 27, 26, 25, 3, 2, 1, 0, 6, 7, 4, 5, \
	0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 0, 1, 6, 7, 5, 4)
#define _trans_cube_DFr _mm256_set_epi8( \
	0, 0, 0, 0, 10, 11, 8, 9, 5, 4, 7, 6, 0, 1, 2, 3, \
	0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 7, 6, 1, 0, 3, 2)
#define _trans_cube_DFr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 10, 11, 8, 9, 5, 4, 7, 6, 0, 1, 2, 3, \
	0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 7, 6, 1, 0, 3, 2)
#define _trans_cube_DLr _mm256_set_epi8( \
	0, 0, 0, 0, 27, 24, 25, 26, 1, 0, 3, 2, 5, 4, 7, 6, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 2, 5, 4, 6, 7)
#define _trans_cube_DLr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 27, 24, 25, 26, 1, 0, 3, 2, 5, 4, 7, 6, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 2, 5, 4, 6, 7)
#define _trans_cube_DBr _mm256_set_epi8( \
	0, 0, 0, 0, 8, 9, 10, 11, 4, 5, 6, 7, 1, 0, 3, 2, \
	0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 7, 0, 1, 2, 3)
#define _trans_cube_DBr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 8, 9, 10, 11, 4, 5, 6, 7, 1, 0, 3, 2, \
	0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 7, 0, 1, 2, 3)
#define _trans_cube_DRr _mm256_set_epi8( \
	0, 0, 0, 0, 25, 26, 27, 24, 0, 1, 2, 3, 4, 5, 6, 7, \
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 3, 4, 5, 7, 6)
#define _trans_cube_DRr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 25, 26, 27, 24, 0, 1, 2, 3, 4, 5, 6, 7, \
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 3, 4, 5, 7, 6)
#define _trans_cube_RUr _mm256_set_epi8( \
	0, 0, 0, 0, 3, 2, 1, 0, 25, 26, 27, 24, 21, 22, 23, 20, \
	0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 38, 37, 66, 65, 67, 64)
#define _trans_cube_RUr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 21, 22, 23, 20, 17, 18, 19, 16, 11, 10, 9, 8, \
	0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 68, 70, 33, 35, 34, 32)
#define _trans_cube_RFr _mm256_set_epi8( \
	0, 0, 0, 0, 18, 17, 16, 19, 22, 21, 20, 23, 25, 26, 27, 24, \
	0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 67, 64, 39, 36, 37, 38)
#define _trans_cube_RFr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 17, 18, 19, 16, 20, 23, 22, 21, 24, 27, 26, 25, \
	0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 65, 66, 37, 38, 39, 36)
#define _trans_cube_RDr _mm256_set_epi8( \
	0, 0, 0, 0, 1, 0, 3, 2, 26, 25, 24, 27, 22, 21, 20, 23, \
	0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 37, 38, 65, 66, 64, 67)
#define _trans_cube_RDr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 20, 23, 22, 21, 16, 19, 18, 17, 9, 8, 11, 10, \
	0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 69, 71, 32, 34, 35, 33)
#define _trans_cube_RBr _mm256_set_epi8( \
	0, 0, 0, 0, 16, 19, 18, 17, 21, 22, 23, 20, 26, 25, 24, 27, \
	0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 64, 67, 36, 39, 38, 37)
#define _trans_cube_RBr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 16, 19, 18, 17, 21, 22, 23, 20, 26, 25, 24, 27, \
	0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 64, 67, 36, 39, 38, 37)
#define _trans_cube_LUr _mm256_set_epi8( \
	0, 0, 0, 0, 2, 3, 0, 1, 27, 24, 25, 26, 20, 23, 22, 21, \
	0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 39, 36, 67, 64, 66, 65)
#define _trans_cube_LUr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 23, 20, 21, 22, 18, 17, 16, 19, 10, 11, 8, 9, \
	0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 70, 68, 35, 33, 32, 34)
#define _trans_cube_LFr _mm256_set_epi8( \
	0, 0, 0, 0, 17, 18, 19, 16, 20, 23, 22, 21, 24, 27, 26, 25, \
	0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 65, 66, 37, 38, 39, 36)
#define _trans_cube_LFr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 18, 17, 16, 19, 22, 21, 20, 23, 25, 26, 27, 24, \
	0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 67, 64, 39, 36, 37, 38)
#define _trans_cube_LDr _mm256_set_epi8( \
	0, 0, 0, 0, 0, 1, 2, 3, 24, 27, 26, 25, 23, 20, 21, 22, \
	0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 36, 39, 64, 67, 65, 66)
#define _trans_cube_LDr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 22, 21, 20, 23, 19, 16, 17, 18, 8, 9, 10, 11, \
	0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 71, 69, 34, 32, 33, 35)
#define _trans_cube_LBr _mm256_set_epi8( \
	0, 0, 0, 0, 19, 16, 17, 18, 23, 20, 21, 22, 27, 24, 25, 26, \
	0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 66, 65, 38, 37, 36, 39)
#define _trans_cube_LBr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 19, 16, 17, 18, 23, 20, 21, 22, 27, 24, 25, 26, \
	0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 66, 65, 38, 37, 36, 39)
#define _trans_cube_FUr _mm256_set_epi8( \
	0, 0, 0, 0, 6, 7, 4, 5, 10, 11, 8, 9, 17, 18, 19, 16, \
	0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 34, 32, 71, 69, 70, 68)
#define _trans_cube_FUr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 6, 7, 4, 5, 10, 11, 8, 9, 17, 18, 19, 16, \
	0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 34, 32, 71, 69, 70, 68)
#define _trans_cube_FRr _mm256_set_epi8( \
	0, 0, 0, 0, 21, 22, 23, 20, 17, 18, 19, 16, 11, 10, 9, 8, \
	0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 68, 70, 33, 35, 34, 32)
#define _trans_cube_FRr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 3, 2, 1, 0, 25, 26, 27, 24, 21, 22, 23, 20, \
	0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 38, 37, 66, 65, 67, 64)
#define _trans_cube_FDr _mm256_set_epi8( \
	0, 0, 0, 0, 4, 5, 6, 7, 11, 10, 9, 8, 18, 17, 16, 19, \
	0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 32, 34, 69, 71, 68, 70)
#define _trans_cube_FDr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 7, 6, 5, 4, 8, 9, 10, 11, 16, 19, 18, 17, \
	0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 35, 33, 70, 68, 71, 69)
#define _trans_cube_FLr _mm256_set_epi8( \
	0, 0, 0, 0, 23, 20, 21, 22, 18, 17, 16, 19, 10, 11, 8, 9, \
	0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 70, 68, 35, 33, 32, 34)
#define _trans_cube_FLr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 2, 3, 0, 1, 27, 24, 25, 26, 20, 23, 22, 21, \
	0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 39, 36, 67, 64, 66, 65)
#define _trans_cube_BUr _mm256_set_epi8( \
	0, 0, 0, 0, 7, 6, 5, 4, 8, 9, 10, 11, 16, 19, 18, 17, \
	0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 35, 33, 70, 68, 71, 69)
#define _trans_cube_BUr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 4, 5, 6, 7, 11, 10, 9, 8, 18, 17, 16, 19, \
	0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 32, 34, 69, 71, 68, 70)
#define _trans_cube_BRr _mm256_set_epi8( \
	0, 0, 0, 0, 22, 21, 20, 23, 19, 16, 17, 18, 8, 9, 10, 11, \
	0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 71, 69, 34, 32, 33, 35)
#define _trans_cube_BRr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 0, 1, 2, 3, 24, 27, 26, 25, 23, 20, 21, 22, \
	0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 36, 39, 64, 67, 65, 66)
#define _trans_cube_BDr _mm256_set_epi8( \
	0, 0, 0, 0, 5, 4, 7, 6, 9, 8, 11, 10, 19, 16, 17, 18, \
	0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 33, 35, 68, 70, 69, 71)
#define _trans_cube_BDr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 5, 4, 7, 6, 9, 8, 11, 10, 19, 16, 17, 18, \
	0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 33, 35, 68, 70, 69, 71)
#define _trans_cube_BLr _mm256_set_epi8( \
	0, 0, 0, 0, 20, 23, 22, 21, 16, 19, 18, 17, 9, 8, 11, 10, \
	0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 69, 71, 32, 34, 35, 33)
#define _trans_cube_BLr_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 1, 0, 3, 2, 26, 25, 24, 27, 22, 21, 20, 23, \
	0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 37, 38, 65, 66, 64, 67)
#define _trans_cube_UFm _mm256_set_epi8( \
	0, 0, 0, 0, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 1, 0, 7, 6, 5, 4)
#define _trans_cube_UFm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 1, 0, 7, 6, 5, 4)
#define _trans_cube_ULm _mm256_set_epi8( \
	0, 0, 0, 0, 25, 26, 27, 24, 3, 2, 1, 0, 7, 6, 5, 4, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 2, 3, 1, 0)
#define _trans_cube_ULm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 25, 26, 27, 24, 3, 2, 1, 0, 7, 6, 5, 4, \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 2, 3, 1, 0)
#define _trans_cube_UBm _mm256_set_epi8( \
	0, 0, 0, 0, 8, 9, 10, 11, 7, 6, 5, 4, 2, 3, 0, 1, \
	0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 1, 6, 7, 4, 5)
#define _trans_cube_UBm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 8, 9, 10, 11, 7, 6, 5, 4, 2, 3, 0, 1, \
	0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 1, 6, 7, 4, 5)
#define _trans_cube_URm _mm256_set_epi8( \
	0, 0, 0, 0, 27, 24, 25, 26, 2, 3, 0, 1, 6, 7, 4, 5, \
	0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 3, 2, 0, 1)
#define _trans_cube_URm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 27, 24, 25, 26, 2, 3, 0, 1, 6, 7, 4, 5, \
	0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 3, 2, 0, 1)
#define _trans_cube_DFm _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 4, 5, 6, 7, 0, 1, 2, 3, \
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 2, 5, 4, 7, 6)
#define _trans_cube_DFm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 11, 10, 9, 8, 4, 5, 6, 7, 0, 1, 2, 3, \
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 2, 5, 4, 7, 6)
#define _trans_cube_DLm _mm256_set_epi8( \
	0, 0, 0, 0, 26, 25, 24, 27, 1, 0, 3, 2, 4, 5, 6, 7, \
	0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 7, 6, 1, 0, 2, 3)
#define _trans_cube_DLm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 24, 27, 26, 25, 0, 1, 2, 3, 5, 4, 7, 6, \
	0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 6, 7, 0, 1, 3, 2)
#define _trans_cube_DBm _mm256_set_epi8( \
	0, 0, 0, 0, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7)
#define _trans_cube_DBm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7)
#define _trans_cube_DRm _mm256_set_epi8( \
	0, 0, 0, 0, 24, 27, 26, 25, 0, 1, 2, 3, 5, 4, 7, 6, \
	0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 6, 7, 0, 1, 3, 2)
#define _trans_cube_DRm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 26, 25, 24, 27, 1, 0, 3, 2, 4, 5, 6, 7, \
	0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 7, 6, 1, 0, 2, 3)
#define _trans_cube_RUm _mm256_set_epi8( \
	0, 0, 0, 0, 3, 2, 1, 0, 24, 27, 26, 25, 20, 23, 22, 21, \
	0, 0, 0, 0, 0, 0, 0, 0, 35, 32, 34, 33, 70, 69, 71, 68)
#define _trans_cube_RUm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 22, 21, 20, 23, 18, 17, 16, 19, 11, 10, 9, 8, \
	0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 34, 32, 71, 69, 68, 70)
#define _trans_cube_RFm _mm256_set_epi8( \
	0, 0, 0, 0, 18, 17, 16, 19, 23, 20, 21, 22, 24, 27, 26, 25, \
	0, 0, 0, 0, 0, 0, 0, 0, 69, 70, 71, 68, 35, 32, 33, 34)
#define _trans_cube_RFm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 18, 17, 16, 19, 23, 20, 21, 22, 24, 27, 26, 25, \
	0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 39, 36, 67, 64, 65, 66)
#define _trans_cube_RDm _mm256_set_epi8( \
	0, 0, 0, 0, 1, 0, 3, 2, 27, 24, 25, 26, 23, 20, 21, 22, \
	0, 0, 0, 0, 0, 0, 0, 0, 32, 35, 33, 34, 69, 70, 68, 71)
#define _trans_cube_RDm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 23, 20, 21, 22, 19, 16, 17, 18, 9, 8, 11, 10, \
	0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 35, 33, 70, 68, 69, 71)
#define _trans_cube_RBm _mm256_set_epi8( \
	0, 0, 0, 0, 16, 19, 18, 17, 20, 23, 22, 21, 27, 24, 25, 26, \
	0, 0, 0, 0, 0, 0, 0, 0, 70, 69, 68, 71, 32, 35, 34, 33)
#define _trans_cube_RBm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 19, 16, 17, 18, 22, 21, 20, 23, 26, 25, 24, 27, \
	0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 38, 37, 66, 65, 64, 67)
#define _trans_cube_LUm _mm256_set_epi8( \
	0, 0, 0, 0, 2, 3, 0, 1, 26, 25, 24, 27, 21, 22, 23, 20, \
	0, 0, 0, 0, 0, 0, 0, 0, 34, 33, 35, 32, 71, 68, 70, 69)
#define _trans_cube_LUm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 20, 23, 22, 21, 17, 18, 19, 16, 10, 11, 8, 9, \
	0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 32, 34, 69, 71, 70, 68)
#define _trans_cube_LFm _mm256_set_epi8( \
	0, 0, 0, 0, 17, 18, 19, 16, 21, 22, 23, 20, 25, 26, 27, 24, \
	0, 0, 0, 0, 0, 0, 0, 0, 71, 68, 69, 70, 33, 34, 35, 32)
#define _trans_cube_LFm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 17, 18, 19, 16, 21, 22, 23, 20, 25, 26, 27, 24, \
	0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 37, 38, 65, 66, 67, 64)
#define _trans_cube_LDm _mm256_set_epi8( \
	0, 0, 0, 0, 0, 1, 2, 3, 25, 26, 27, 24, 22, 21, 20, 23, \
	0, 0, 0, 0, 0, 0, 0, 0, 33, 34, 32, 35, 68, 71, 69, 70)
#define _trans_cube_LDm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 21, 22, 23, 20, 16, 19, 18, 17, 8, 9, 10, 11, \
	0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 33, 35, 68, 70, 71, 69)
#define _trans_cube_LBm _mm256_set_epi8( \
	0, 0, 0, 0, 19, 16, 17, 18, 22, 21, 20, 23, 26, 25, 24, 27, \
	0, 0, 0, 0, 0, 0, 0, 0, 68, 71, 70, 69, 34, 33, 32, 35)
#define _trans_cube_LBm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 16, 19, 18, 17, 20, 23, 22, 21, 27, 24, 25, 26, \
	0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 36, 39, 64, 67, 66, 65)
#define _trans_cube_FUm _mm256_set_epi8( \
	0, 0, 0, 0, 7, 6, 5, 4, 11, 10, 9, 8, 17, 18, 19, 16, \
	0, 0, 0, 0, 0, 0, 0, 0, 39, 37, 38, 36, 67, 65, 66, 64)
#define _trans_cube_FUm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 7, 6, 5, 4, 11, 10, 9, 8, 17, 18, 19, 16, \
	0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 70, 68, 35, 33, 34, 32)
#define _trans_cube_FRm _mm256_set_epi8( \
	0, 0, 0, 0, 20, 23, 22, 21, 17, 18, 19, 16, 10, 11, 8, 9, \
	0, 0, 0, 0, 0, 0, 0, 0, 67, 65, 64, 66, 37, 39, 38, 36)
#define _trans_cube_FRm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 2, 3, 0, 1, 26, 25, 24, 27, 21, 22, 23, 20, \
	0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 67, 64, 39, 36, 38, 37)
#define _trans_cube_FDm _mm256_set_epi8( \
	0, 0, 0, 0, 5, 4, 7, 6, 10, 11, 8, 9, 18, 17, 16, 19, \
	0, 0, 0, 0, 0, 0, 0, 0, 37, 39, 36, 38, 65, 67, 64, 66)
#define _trans_cube_FDm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 6, 7, 4, 5, 9, 8, 11, 10, 16, 19, 18, 17, \
	0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 71, 69, 34, 32, 35, 33)
#define _trans_cube_FLm _mm256_set_epi8( \
	0, 0, 0, 0, 22, 21, 20, 23, 18, 17, 16, 19, 11, 10, 9, 8, \
	0, 0, 0, 0, 0, 0, 0, 0, 65, 67, 66, 64, 39, 37, 36, 38)
#define _trans_cube_FLm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 3, 2, 1, 0, 24, 27, 26, 25, 20, 23, 22, 21, \
	0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 66, 65, 38, 37, 39, 36)
#define _trans_cube_BUm _mm256_set_epi8( \
	0, 0, 0, 0, 6, 7, 4, 5, 9, 8, 11, 10, 16, 19, 18, 17, \
	0, 0, 0, 0, 0, 0, 0, 0, 38, 36, 39, 37, 66, 64, 67, 65)
#define _trans_cube_BUm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 5, 4, 7, 6, 10, 11, 8, 9, 18, 17, 16, 19, \
	0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 68, 70, 33, 35, 32, 34)
#define _trans_cube_BRm _mm256_set_epi8( \
	0, 0, 0, 0, 23, 20, 21, 22, 19, 16, 17, 18, 9, 8, 11, 10, \
	0, 0, 0, 0, 0, 0, 0, 0, 64, 66, 67, 65, 38, 36, 37, 39)
#define _trans_cube_BRm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 1, 0, 3, 2, 27, 24, 25, 26, 23, 20, 21, 22, \
	0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 65, 66, 37, 38, 36, 39)
#define _trans_cube_BDm _mm256_set_epi8( \
	0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 19, 16, 17, 18, \
	0, 0, 0, 0, 0, 0, 0, 0, 36, 38, 37, 39, 64, 66, 65, 67)
#define _trans_cube_BDm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 19, 16, 17, 18, \
	0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 69, 71, 32, 34, 33, 35)
#define _trans_cube_BLm _mm256_set_epi8( \
	0, 0, 0, 0, 21, 22, 23, 20, 16, 19, 18, 17, 8, 9, 10, 11, \
	0, 0, 0, 0, 0, 0, 0, 0, 66, 64, 65, 67, 36, 38, 39, 37)
#define _trans_cube_BLm_inverse _mm256_set_epi8( \
	0, 0, 0, 0, 0, 1, 2, 3, 25, 26, 27, 24, 22, 21, 20, 23, \
	0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 64, 67, 36, 39, 37, 38)

_static cube_fast_t cubetofast(cube_t);
_static cube_t fasttocube(cube_fast_t);
_static_inline bool equal_fast(cube_fast_t, cube_fast_t);
_static_inline bool issolved_fast(cube_fast_t);
_static_inline cube_fast_t invertco_fast(cube_fast_t);
_static_inline cube_fast_t cleanaftershuffle(cube_fast_t);
_static_inline cube_fast_t inverse_fast(cube_fast_t);
_static_inline cube_fast_t compose_fast(cube_fast_t, cube_fast_t);
_static_inline int64_t coord_fast_eo(cube_fast_t);

_static cube_fast_t
cubetofast(cube_t a)
{
	uint8_t aux[32];

	memset(aux, 0, 32);
	memcpy(aux, &a.corner, 8);
	memcpy(aux + 16, &a.edge, 12);

	return _mm256_loadu_si256((__m256i_u *)&aux);
}

_static cube_t
fasttocube(cube_fast_t c)
{
	cube_t a;
	uint8_t aux[32];

	_mm256_storeu_si256((__m256i_u *)aux, c);
	memcpy(&a.corner, aux, 8);
	memcpy(&a.edge, aux + 16, 12);

	return a;
}

_static_inline bool
equal_fast(cube_fast_t c1, cube_fast_t c2)
{
	int32_t mask;
	__m256i cmp;

	cmp = _mm256_cmpeq_epi8(c1, c2);
	mask = _mm256_movemask_epi8(cmp);

	return mask == ~0;
}

_static_inline bool
issolved_fast(cube_fast_t cube)
{
	return equal_fast(cube, solved_fast);
}

_static_inline cube_fast_t
invertco_fast(cube_fast_t c)
{
        cube_fast_t co, shleft, shright, summed, newco, cleanco, ret;

        co = _mm256_and_si256(c, _co2_avx2);
        shleft = _mm256_slli_epi32(co, 1);
        shright = _mm256_srli_epi32(co, 1);
        summed = _mm256_or_si256(shleft, shright);
        newco = _mm256_and_si256(summed, _co2_avx2);
        cleanco = _mm256_xor_si256(c, co);
        ret = _mm256_or_si256(cleanco, newco);

        return ret;
}

_static_inline cube_fast_t
cleanaftershuffle(cube_fast_t c)
{
	__m256i b;

	b = _mm256_set_epi8(
		~0, ~0, ~0, ~0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 0, 0, 0, 0, 0, 0, 0, 0
	);

	return _mm256_andnot_si256(b, c);
}

_static_inline cube_fast_t
inverse_fast(cube_fast_t c)
{
	/* Method taken from Andrew Skalski's vcube[1]. The addition sequence
	 * was generated using [2].
	 * [1] https://github.com/Voltara/vcube
	 * [2] http://wwwhomes.uni-bielefeld.de/achim/addition_chain.html
	 */
	cube_fast_t v3, vi, vo, vp, ret;

	v3 = _mm256_shuffle_epi8(c, c);
	v3 = _mm256_shuffle_epi8(v3, c);
	vi = _mm256_shuffle_epi8(v3, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, c);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, c);

	vo = _mm256_and_si256(c, _mm256_or_si256(_eo_avx2, _co2_avx2));
	vo = _mm256_shuffle_epi8(vo, vi);
	vp = _mm256_andnot_si256(_mm256_or_si256(_eo_avx2, _co2_avx2), vi);
	ret = _mm256_or_si256(vp, vo);
	ret = cleanaftershuffle(ret);
	
	return invertco_fast(ret);
}

_static_inline cube_fast_t
compose_fast(cube_fast_t c1, cube_fast_t c2)
{
	cube_fast_t ret;

	cube_fast_t s, eo2, ed, co1, co2, aux, auy1, auy2, auz1, auz2, coclean;

	eo2 = _mm256_and_si256(c2, _eo_avx2);
	s = _mm256_shuffle_epi8(c1, c2);
	s = cleanaftershuffle(s);
	ed = _mm256_xor_si256(s, eo2);
	co1 = _mm256_and_si256(s, _co2_avx2);
	co2 = _mm256_and_si256(c2, _co2_avx2);
	aux = _mm256_add_epi8(co1, co2);
	auy1 = _mm256_add_epi8(aux, _cocw_avx2);
	auy2 = _mm256_srli_epi32(auy1, 2);
	auz1 = _mm256_add_epi8(aux, auy2);
	auz2 = _mm256_and_si256(auz1, _co2_avx2);
	coclean = _mm256_andnot_si256(_co2_avx2, ed);
	ret = _mm256_or_si256(coclean, auz2);

	return ret;
}

_static_inline int64_t
coord_fast_eo(cube_fast_t c)
{
	cube_fast_t eo, shifted;
	int64_t mask;

	eo = _mm256_and_si256(c, _eo_avx2);
	shifted = _mm256_slli_epi32(eo, 3);
	mask = _mm256_movemask_epi8(shifted);

	return mask >> 17;
}


/******************************************************************************
Section: portable fast methods

This section contains performance-critical methods that do not use
advanced CPU instructions. They are used as an alternative to the ones
in the previous section(s) for unsupported architectures.
******************************************************************************/

#else

typedef cube_t cube_fast_t;

_static cube_fast_t zero_fast = { .corner = {0}, .edge = {0} };
_static cube_t solved_fast = {
	.corner = {0, 1, 2, 3, 4, 5, 6, 7},
	.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
};

_static cube_fast_t _move_cube_U = {
	.corner = {5, 4, 2, 3, 0, 1, 6, 7},
	.edge = {4, 5, 2, 3, 1, 0, 6, 7, 8, 9, 10, 11} };
_static cube_fast_t _move_cube_U2 = {
	.corner = {1, 0, 2, 3, 5, 4, 6, 7},
	.edge = {1, 0, 2, 3, 5, 4, 6, 7, 8, 9, 10, 11} };
_static cube_fast_t _move_cube_U3 = {
	.corner = {4, 5, 2, 3, 1, 0, 6, 7},
	.edge = {5, 4, 2, 3, 0, 1, 6, 7, 8, 9, 10, 11} };
_static cube_fast_t _move_cube_D = {
	.corner = {0, 1, 7, 6, 4, 5, 2, 3},
	.edge = {0, 1, 7, 6, 4, 5, 2, 3, 8, 9, 10, 11} };
_static cube_fast_t _move_cube_D2 = {
	.corner = {0, 1, 3, 2, 4, 5, 7, 6},
	.edge = {0, 1, 3, 2, 4, 5, 7, 6, 8, 9, 10, 11} };
_static cube_fast_t _move_cube_D3 = {
	.corner = {0, 1, 6, 7, 4, 5, 3, 2},
	.edge = {0, 1, 6, 7, 4, 5, 3, 2, 8, 9, 10, 11} };
_static cube_fast_t _move_cube_R = {
	.corner = {70, 1, 2, 69, 4, 32, 35, 7},
	.edge = {0, 1, 2, 3, 8, 5, 6, 11, 7, 9, 10, 4} };
_static cube_fast_t _move_cube_R2 = {
	.corner = {3, 1, 2, 0, 4, 6, 5, 7},
	.edge = {0, 1, 2, 3, 7, 5, 6, 4, 11, 9, 10, 8} };
_static cube_fast_t _move_cube_R3 = {
	.corner = {69, 1, 2, 70, 4, 35, 32, 7},
	.edge = {0, 1, 2, 3, 11, 5, 6, 8, 4, 9, 10, 7} };
_static cube_fast_t _move_cube_L = {
	.corner = {0, 71, 68, 3, 33, 5, 6, 34},
	.edge = {0, 1, 2, 3, 4, 10, 9, 7, 8, 5, 6, 11} };
_static cube_fast_t _move_cube_L2 = {
	.corner = {0, 2, 1, 3, 7, 5, 6, 4},
	.edge = {0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11} };
_static cube_fast_t _move_cube_L3 = {
	.corner = {0, 68, 71, 3, 34, 5, 6, 33},
	.edge = {0, 1, 2, 3, 4, 9, 10, 7, 8, 6, 5, 11} };
_static cube_fast_t _move_cube_F = {
	.corner = {36, 1, 38, 3, 66, 5, 64, 7},
	.edge = {25, 1, 2, 24, 4, 5, 6, 7, 16, 19, 10, 11} };
_static cube_fast_t _move_cube_F2 = {
	.corner = {2, 1, 0, 3, 6, 5, 4, 7},
	.edge = {3, 1, 2, 0, 4, 5, 6, 7, 9, 8, 10, 11} };
_static cube_fast_t _move_cube_F3 = {
	.corner = {38, 1, 36, 3, 64, 5, 66, 7},
	.edge = {24, 1, 2, 25, 4, 5, 6, 7, 19, 16, 10, 11} };
_static cube_fast_t _move_cube_B = {
	.corner = {0, 37, 2, 39, 4, 67, 6, 65},
	.edge = {0, 27, 26, 3, 4, 5, 6, 7, 8, 9, 17, 18} };
_static cube_fast_t _move_cube_B2 = {
	.corner = {0, 3, 2, 1, 4, 7, 6, 5},
	.edge = {0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 11, 10} };
_static cube_fast_t _move_cube_B3 = {
	.corner = {0, 39, 2, 37, 4, 65, 6, 67},
	.edge = {0, 26, 27, 3, 4, 5, 6, 7, 8, 9, 18, 17} };

_static cube_fast_t _trans_cube_UFr = {
	.corner = {0, 1, 2, 3, 4, 5, 6, 7},
	.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11} };
_static cube_fast_t _trans_cube_UFr_inverse = {
	.corner = {0, 1, 2, 3, 4, 5, 6, 7},
	.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11} };
_static cube_fast_t _trans_cube_ULr = {
	.corner = {4, 5, 7, 6, 1, 0, 2, 3},
	.edge = {5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24} };
_static cube_fast_t _trans_cube_ULr_inverse = {
	.corner = {5, 4, 6, 7, 0, 1, 3, 2},
	.edge = {4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26} };
_static cube_fast_t _trans_cube_UBr = {
	.corner = {1, 0, 3, 2, 5, 4, 7, 6},
	.edge = {1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9} };
_static cube_fast_t _trans_cube_UBr_inverse = {
	.corner = {1, 0, 3, 2, 5, 4, 7, 6},
	.edge = {1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9} };
_static cube_fast_t _trans_cube_URr = {
	.corner = {5, 4, 6, 7, 0, 1, 3, 2},
	.edge = {4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26} };
_static cube_fast_t _trans_cube_URr_inverse = {
	.corner = {4, 5, 7, 6, 1, 0, 2, 3},
	.edge = {5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24} };
_static cube_fast_t _trans_cube_DFr = {
	.corner = {2, 3, 0, 1, 6, 7, 4, 5},
	.edge = {3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10} };
_static cube_fast_t _trans_cube_DFr_inverse = {
	.corner = {2, 3, 0, 1, 6, 7, 4, 5},
	.edge = {3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10} };
_static cube_fast_t _trans_cube_DLr = {
	.corner = {7, 6, 4, 5, 2, 3, 1, 0},
	.edge = {6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27} };
_static cube_fast_t _trans_cube_DLr_inverse = {
	.corner = {7, 6, 4, 5, 2, 3, 1, 0},
	.edge = {6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27} };
_static cube_fast_t _trans_cube_DBr = {
	.corner = {3, 2, 1, 0, 7, 6, 5, 4},
	.edge = {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8} };
_static cube_fast_t _trans_cube_DBr_inverse = {
	.corner = {3, 2, 1, 0, 7, 6, 5, 4},
	.edge = {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8} };
_static cube_fast_t _trans_cube_DRr = {
	.corner = {6, 7, 5, 4, 3, 2, 0, 1},
	.edge = {7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25} };
_static cube_fast_t _trans_cube_DRr_inverse = {
	.corner = {6, 7, 5, 4, 3, 2, 0, 1},
	.edge = {7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25} };
_static cube_fast_t _trans_cube_RUr = {
	.corner = {64, 67, 65, 66, 37, 38, 36, 39},
	.edge = {20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3} };
_static cube_fast_t _trans_cube_RUr_inverse = {
	.corner = {32, 34, 35, 33, 70, 68, 69, 71},
	.edge = {8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21} };
_static cube_fast_t _trans_cube_RFr = {
	.corner = {38, 37, 36, 39, 64, 67, 66, 65},
	.edge = {24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18} };
_static cube_fast_t _trans_cube_RFr_inverse = {
	.corner = {36, 39, 38, 37, 66, 65, 64, 67},
	.edge = {25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17} };
_static cube_fast_t _trans_cube_RDr = {
	.corner = {67, 64, 66, 65, 38, 37, 39, 36},
	.edge = {23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1} };
_static cube_fast_t _trans_cube_RDr_inverse = {
	.corner = {33, 35, 34, 32, 71, 69, 68, 70},
	.edge = {10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20} };
_static cube_fast_t _trans_cube_RBr = {
	.corner = {37, 38, 39, 36, 67, 64, 65, 66},
	.edge = {27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16} };
_static cube_fast_t _trans_cube_RBr_inverse = {
	.corner = {37, 38, 39, 36, 67, 64, 65, 66},
	.edge = {27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16} };
_static cube_fast_t _trans_cube_LUr = {
	.corner = {65, 66, 64, 67, 36, 39, 37, 38},
	.edge = {21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2} };
_static cube_fast_t _trans_cube_LUr_inverse = {
	.corner = {34, 32, 33, 35, 68, 70, 71, 69},
	.edge = {9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23} };
_static cube_fast_t _trans_cube_LFr = {
	.corner = {36, 39, 38, 37, 66, 65, 64, 67},
	.edge = {25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17} };
_static cube_fast_t _trans_cube_LFr_inverse = {
	.corner = {38, 37, 36, 39, 64, 67, 66, 65},
	.edge = {24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18} };
_static cube_fast_t _trans_cube_LDr = {
	.corner = {66, 65, 67, 64, 39, 36, 38, 37},
	.edge = {22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0} };
_static cube_fast_t _trans_cube_LDr_inverse = {
	.corner = {35, 33, 32, 34, 69, 71, 70, 68},
	.edge = {11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22} };
_static cube_fast_t _trans_cube_LBr = {
	.corner = {39, 36, 37, 38, 65, 66, 67, 64},
	.edge = {26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19} };
_static cube_fast_t _trans_cube_LBr_inverse = {
	.corner = {39, 36, 37, 38, 65, 66, 67, 64},
	.edge = {26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19} };
_static cube_fast_t _trans_cube_FUr = {
	.corner = {68, 70, 69, 71, 32, 34, 33, 35},
	.edge = {16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6} };
_static cube_fast_t _trans_cube_FUr_inverse = {
	.corner = {68, 70, 69, 71, 32, 34, 33, 35},
	.edge = {16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6} };
_static cube_fast_t _trans_cube_FRr = {
	.corner = {32, 34, 35, 33, 70, 68, 69, 71},
	.edge = {8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21} };
_static cube_fast_t _trans_cube_FRr_inverse = {
	.corner = {64, 67, 65, 66, 37, 38, 36, 39},
	.edge = {20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3} };
_static cube_fast_t _trans_cube_FDr = {
	.corner = {70, 68, 71, 69, 34, 32, 35, 33},
	.edge = {19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4} };
_static cube_fast_t _trans_cube_FDr_inverse = {
	.corner = {69, 71, 68, 70, 33, 35, 32, 34},
	.edge = {17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7} };
_static cube_fast_t _trans_cube_FLr = {
	.corner = {34, 32, 33, 35, 68, 70, 71, 69},
	.edge = {9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23} };
_static cube_fast_t _trans_cube_FLr_inverse = {
	.corner = {65, 66, 64, 67, 36, 39, 37, 38},
	.edge = {21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2} };
_static cube_fast_t _trans_cube_BUr = {
	.corner = {69, 71, 68, 70, 33, 35, 32, 34},
	.edge = {17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7} };
_static cube_fast_t _trans_cube_BUr_inverse = {
	.corner = {70, 68, 71, 69, 34, 32, 35, 33},
	.edge = {19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4} };
_static cube_fast_t _trans_cube_BRr = {
	.corner = {35, 33, 32, 34, 69, 71, 70, 68},
	.edge = {11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22} };
_static cube_fast_t _trans_cube_BRr_inverse = {
	.corner = {66, 65, 67, 64, 39, 36, 38, 37},
	.edge = {22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0} };
_static cube_fast_t _trans_cube_BDr = {
	.corner = {71, 69, 70, 68, 35, 33, 34, 32},
	.edge = {18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5} };
_static cube_fast_t _trans_cube_BDr_inverse = {
	.corner = {71, 69, 70, 68, 35, 33, 34, 32},
	.edge = {18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5} };
_static cube_fast_t _trans_cube_BLr = {
	.corner = {33, 35, 34, 32, 71, 69, 68, 70},
	.edge = {10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20} };
_static cube_fast_t _trans_cube_BLr_inverse = {
	.corner = {67, 64, 66, 65, 38, 37, 39, 36},
	.edge = {23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1} };
_static cube_fast_t _trans_cube_UFm = {
	.corner = {4, 5, 6, 7, 0, 1, 2, 3},
	.edge = {0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10} };
_static cube_fast_t _trans_cube_UFm_inverse = {
	.corner = {4, 5, 6, 7, 0, 1, 2, 3},
	.edge = {0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10} };
_static cube_fast_t _trans_cube_ULm = {
	.corner = {0, 1, 3, 2, 5, 4, 6, 7},
	.edge = {4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25} };
_static cube_fast_t _trans_cube_ULm_inverse = {
	.corner = {0, 1, 3, 2, 5, 4, 6, 7},
	.edge = {4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25} };
_static cube_fast_t _trans_cube_UBm = {
	.corner = {5, 4, 7, 6, 1, 0, 3, 2},
	.edge = {1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8} };
_static cube_fast_t _trans_cube_UBm_inverse = {
	.corner = {5, 4, 7, 6, 1, 0, 3, 2},
	.edge = {1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8} };
_static cube_fast_t _trans_cube_URm = {
	.corner = {1, 0, 2, 3, 4, 5, 7, 6},
	.edge = {5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27} };
_static cube_fast_t _trans_cube_URm_inverse = {
	.corner = {1, 0, 2, 3, 4, 5, 7, 6},
	.edge = {5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27} };
_static cube_fast_t _trans_cube_DFm = {
	.corner = {6, 7, 4, 5, 2, 3, 0, 1},
	.edge = {3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11} };
_static cube_fast_t _trans_cube_DFm_inverse = {
	.corner = {6, 7, 4, 5, 2, 3, 0, 1},
	.edge = {3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11} };
_static cube_fast_t _trans_cube_DLm = {
	.corner = {3, 2, 0, 1, 6, 7, 5, 4},
	.edge = {7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26} };
_static cube_fast_t _trans_cube_DLm_inverse = {
	.corner = {2, 3, 1, 0, 7, 6, 4, 5},
	.edge = {6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24} };
_static cube_fast_t _trans_cube_DBm = {
	.corner = {7, 6, 5, 4, 3, 2, 1, 0},
	.edge = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9} };
_static cube_fast_t _trans_cube_DBm_inverse = {
	.corner = {7, 6, 5, 4, 3, 2, 1, 0},
	.edge = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9} };
_static cube_fast_t _trans_cube_DRm = {
	.corner = {2, 3, 1, 0, 7, 6, 4, 5},
	.edge = {6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24} };
_static cube_fast_t _trans_cube_DRm_inverse = {
	.corner = {3, 2, 0, 1, 6, 7, 5, 4},
	.edge = {7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26} };
_static cube_fast_t _trans_cube_RUm = {
	.corner = {68, 71, 69, 70, 33, 34, 32, 35},
	.edge = {21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3} };
_static cube_fast_t _trans_cube_RUm_inverse = {
	.corner = {70, 68, 69, 71, 32, 34, 35, 33},
	.edge = {8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22} };
_static cube_fast_t _trans_cube_RFm = {
	.corner = {34, 33, 32, 35, 68, 71, 70, 69},
	.edge = {25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18} };
_static cube_fast_t _trans_cube_RFm_inverse = {
	.corner = {66, 65, 64, 67, 36, 39, 38, 37},
	.edge = {25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18} };
_static cube_fast_t _trans_cube_RDm = {
	.corner = {71, 68, 70, 69, 34, 33, 35, 32},
	.edge = {22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1} };
_static cube_fast_t _trans_cube_RDm_inverse = {
	.corner = {71, 69, 68, 70, 33, 35, 34, 32},
	.edge = {10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23} };
_static cube_fast_t _trans_cube_RBm = {
	.corner = {33, 34, 35, 32, 71, 68, 69, 70},
	.edge = {26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16} };
_static cube_fast_t _trans_cube_RBm_inverse = {
	.corner = {67, 64, 65, 66, 37, 38, 39, 36},
	.edge = {27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19} };
_static cube_fast_t _trans_cube_LUm = {
	.corner = {69, 70, 68, 71, 32, 35, 33, 34},
	.edge = {20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2} };
_static cube_fast_t _trans_cube_LUm_inverse = {
	.corner = {68, 70, 71, 69, 34, 32, 33, 35},
	.edge = {9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20} };
_static cube_fast_t _trans_cube_LFm = {
	.corner = {32, 35, 34, 33, 70, 69, 68, 71},
	.edge = {24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17} };
_static cube_fast_t _trans_cube_LFm_inverse = {
	.corner = {64, 67, 66, 65, 38, 37, 36, 39},
	.edge = {24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17} };
_static cube_fast_t _trans_cube_LDm = {
	.corner = {70, 69, 71, 68, 35, 32, 34, 33},
	.edge = {23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0} };
_static cube_fast_t _trans_cube_LDm_inverse = {
	.corner = {69, 71, 70, 68, 35, 33, 32, 34},
	.edge = {11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21} };
_static cube_fast_t _trans_cube_LBm = {
	.corner = {35, 32, 33, 34, 69, 70, 71, 68},
	.edge = {27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19} };
_static cube_fast_t _trans_cube_LBm_inverse = {
	.corner = {65, 66, 67, 64, 39, 36, 37, 38},
	.edge = {26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16} };
_static cube_fast_t _trans_cube_FUm = {
	.corner = {64, 66, 65, 67, 36, 38, 37, 39},
	.edge = {16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7} };
_static cube_fast_t _trans_cube_FUm_inverse = {
	.corner = {32, 34, 33, 35, 68, 70, 69, 71},
	.edge = {16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7} };
_static cube_fast_t _trans_cube_FRm = {
	.corner = {36, 38, 39, 37, 66, 64, 65, 67},
	.edge = {9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20} };
_static cube_fast_t _trans_cube_FRm_inverse = {
	.corner = {37, 38, 36, 39, 64, 67, 65, 66},
	.edge = {20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2} };
_static cube_fast_t _trans_cube_FDm = {
	.corner = {66, 64, 67, 65, 38, 36, 39, 37},
	.edge = {19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5} };
_static cube_fast_t _trans_cube_FDm_inverse = {
	.corner = {33, 35, 32, 34, 69, 71, 68, 70},
	.edge = {17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6} };
_static cube_fast_t _trans_cube_FLm = {
	.corner = {38, 36, 37, 39, 64, 66, 67, 65},
	.edge = {8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22} };
_static cube_fast_t _trans_cube_FLm_inverse = {
	.corner = {36, 39, 37, 38, 65, 66, 64, 67},
	.edge = {21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3} };
_static cube_fast_t _trans_cube_BUm = {
	.corner = {65, 67, 64, 66, 37, 39, 36, 38},
	.edge = {17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6} };
_static cube_fast_t _trans_cube_BUm_inverse = {
	.corner = {34, 32, 35, 33, 70, 68, 71, 69},
	.edge = {19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5} };
_static cube_fast_t _trans_cube_BRm = {
	.corner = {39, 37, 36, 38, 65, 67, 66, 64},
	.edge = {10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23} };
_static cube_fast_t _trans_cube_BRm_inverse = {
	.corner = {39, 36, 38, 37, 66, 65, 67, 64},
	.edge = {22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1} };
_static cube_fast_t _trans_cube_BDm = {
	.corner = {67, 65, 66, 64, 39, 37, 38, 36},
	.edge = {18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4} };
_static cube_fast_t _trans_cube_BDm_inverse = {
	.corner = {35, 33, 34, 32, 71, 69, 70, 68},
	.edge = {18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4} };
_static cube_fast_t _trans_cube_BLm = {
	.corner = {37, 39, 38, 36, 67, 65, 64, 66},
	.edge = {11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21} };
_static cube_fast_t _trans_cube_BLm_inverse = {
	.corner = {38, 37, 39, 36, 67, 64, 66, 65},
	.edge = {23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0} };

_static cube_fast_t cubetofast(cube_t);
_static cube_t fasttocube(cube_fast_t);
_static_inline bool equal_fast(cube_fast_t, cube_fast_t);
_static_inline bool issolved_fast(cube_fast_t);
_static_inline cube_fast_t invertco_fast(cube_fast_t);
_static_inline cube_fast_t inverse_fast(cube_fast_t);
_static_inline cube_fast_t compose_fast(cube_fast_t, cube_fast_t);
_static_inline int64_t coord_fast_eo(cube_fast_t);

_static cube_fast_t
cubetofast(cube_t cube)
{
	cube_fast_t fast;
	memcpy(&fast, &cube, sizeof(cube_fast_t));
	return fast;
}

_static cube_t
fasttocube(cube_fast_t fast)
{
	cube_t cube;
	memcpy(&cube, &fast, sizeof(cube_fast_t));
	return cube;
}

_static_inline bool
equal_fast(cube_fast_t c1, cube_fast_t c2)
{
	uint8_t i;
	bool ret;

	ret = true;
	for (i = 0; i < 8; i++)
		ret = ret && c1.corner[i] == c2.corner[i];
	for (i = 0; i < 12; i++)
		ret = ret && c1.edge[i] == c2.edge[i];

	return ret;
}

_static_inline bool
issolved_fast(cube_fast_t cube)
{
	return equal_fast(cube, solved_fast);
}

_static_inline cube_fast_t
invertco_fast(cube_fast_t c)
{
	uint8_t i, piece, orien;
	cube_fast_t ret;

	ret = c;
	for (i = 0; i < 8; i++) {
		piece = c.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.corner[i] = (piece & _pbits) | orien;
	}

	return ret;
}

_static_inline cube_fast_t
inverse_fast(cube_fast_t cube)
{
	cube_fast_t ret;
	uint8_t i, piece, orien;

	ret = zero_fast;

	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		orien = piece & _eobit;
		ret.edge[piece & _pbits] = i | orien;
	}

	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.corner[piece & _pbits] = i | orien;
	}

	return ret;
}

_static_inline cube_fast_t
compose_fast(cube_fast_t c1, cube_fast_t c2)
{
	cube_fast_t ret;
	uint8_t i, piece1, piece2, p, orien, aux, auy;

	ret = zero_fast;

	for (i = 0; i < 12; i++) {
		piece2 = c2.edge[i];
		p = piece2 & _pbits;
		piece1 = c1.edge[p];
		orien = (piece2 ^ piece1) & _eobit;
		ret.edge[i] = (piece1 & _pbits) | orien;
	}

	for (i = 0; i < 8; i++) {
		piece2 = c2.corner[i];
		p = piece2 & _pbits;
		piece1 = c1.corner[p];
		aux = (piece2 & _cobits) + (piece1 & _cobits);
		auy = (aux + _ctwist_cw) >> 2U;
		orien = (aux + auy) & _cobits2;
		ret.corner[i] = (piece1 & _pbits) | orien;
	}

	return ret;
}

_static_inline int64_t
coord_fast_eo(cube_fast_t cube)
{
	int i, p;
	int64_t ret;

	ret = 0;
	for (i = 1, p = 1; i < 12; i++, p *= 2)
		ret += p * (cube.edge[i] >> 4);

	return ret;
}

#endif

/******************************************************************************
Section: generic methods

This section contains generic functionality, including the public functions.
Some of these routines depend on the efficient functions implemented in the
previous sections, while some other operate directly on the cube.
******************************************************************************/

#define _move(M, c) compose_fast(c, _move_cube_ ## M)
#define _premove(M, c) compose_fast(_move_cube_ ## M, c)
#define _trans_rotation(T, c) \
	compose_fast(compose_fast(_trans_cube_ ## T, c), \
	_trans_cube_ ## T ## _inverse)
#define _trans_mirrored(T, c) \
	invertco_fast(compose_fast(compose_fast(_trans_cube_ ## T, c), \
	_trans_cube_ ## T ## _inverse))

_static cube_t zero = { .corner = {0}, .edge = {0} };
_static cube_t solved = {
	.corner = {0, 1, 2, 3, 4, 5, 6, 7},
	.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
};

cube_t solvedcube(void);
bool isconsistent(cube_t);
bool issolvable(cube_t);
bool issolved(cube_t cube);
bool equal(cube_t, cube_t);
bool iserror(cube_t);
cube_t compose(cube_t, cube_t);
cube_t inverse(cube_t);
cube_t applymoves(cube_t, char *);
cube_t applytrans(cube_t, char *);
cube_t readcube(char *, char *);
void writecube(char *, cube_t, char *);

_static int permsign(uint8_t *, int);
_static uint8_t readco(char *);
_static uint8_t readcp(char *);
_static uint8_t readeo(char *);
_static uint8_t readep(char *);
_static cube_t readcube_H48(char *);
_static int writepiece_SRC(uint8_t, char *);
_static void writecube_AVX(cube_t, char *);
_static void writecube_H48(cube_t, char *);
_static void writecube_SRC(cube_t, char *);
_static uint8_t readmove(char);
_static uint8_t readmodifier(char);
_static uint8_t readtrans(char *);
_static int writemoves(uint8_t *, int, char *);
_static void writetrans(uint8_t, char *);
_static cube_fast_t move(cube_fast_t, uint8_t);
_static cube_fast_t transform(cube_fast_t, uint8_t);

cube_t
solvedcube(void)
{
	return solved;
}

bool
isconsistent(cube_t cube)
{
	uint8_t i, p, e, piece;
	bool found[12];

	for (i = 0; i < 12; i++)
		found[i] = false;
	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		p = piece & _pbits;
		e = piece & _eobit;
		if (p >= 12)
			goto inconsistent_ep;
		if (e != 0 && e != _eobit)
			goto inconsistent_eo;
		found[p] = true;
	}
	for (i = 0; i < 12; i++)
		if (!found[i])
			goto inconsistent_ep;

	for (i = 0; i < 8; i++)
		found[i] = false;
	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		p = piece & _pbits;
		e = piece & _cobits;
		if (p >= 8)
			goto inconsistent_cp;
		if (e != 0 && e != _ctwist_cw && e != _ctwist_ccw)
			goto inconsistent_co;
		found[p] = true;
	}
	for (i = 0; i < 8; i++)
		if (!found[i])
			goto inconsistent_co;

	return true;

inconsistent_ep:
	DBG_LOG("Inconsistent EP\n");
	return false;
inconsistent_cp:
	DBG_LOG("Inconsistent CP\n");
	return false;
inconsistent_eo:
	DBG_LOG("Inconsistent EO\n");
	return false;
inconsistent_co:
	DBG_LOG("Inconsistent CO\n");
	return false;
}

bool
issolvable(cube_t cube)
{
	uint8_t i, eo, co, piece, edges[12], corners[8];

	DBG_ASSERT(isconsistent(cube), false,
	    "issolvable: cube is inconsistent\n");

	for (i = 0; i < 12; i++)
		edges[i] = cube.edge[i] & _pbits;
	for (i = 0; i < 8; i++)
		corners[i] = cube.corner[i] & _pbits;

	if (permsign(edges, 12) != permsign(corners, 8))
		goto issolvable_parity;

	eo = 0;
	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		eo += (piece & _eobit) >> _eoshift;
	}
	if (eo % 2 != 0)
		goto issolvable_eo;

	co = 0;
	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		co += (piece & _cobits) >> _coshift;
	}
	if (co % 3 != 0)
		goto issolvable_co;

	return true;

issolvable_parity:
	DBG_LOG("EP and CP parities are different\n");
	return false;
issolvable_eo:
	DBG_LOG("Odd number of flipped edges\n");
	return false;
issolvable_co:
	DBG_LOG("Sum of corner orientation is not multiple of 3\n");
	return false;
}

bool
issolved(cube_t cube)
{
	return equal(cube, solved);
}

bool
equal(cube_t c1, cube_t c2)
{
	int i;
	bool ret;

	ret = true;
	for (i = 0; i < 8; i++)
		ret = ret && c1.corner[i] == c2.corner[i];
	for (i = 0; i < 12; i++)
		ret = ret && c1.edge[i] == c2.edge[i];

	return ret;
}

bool
iserror(cube_t cube)
{
	return equal(cube, zero);
}

cube_t
compose(cube_t c1, cube_t c2)
{
	DBG_ASSERT(isconsistent(c1) && isconsistent(c2),
	    zero, "compose error: inconsistent cube\n")

	return fasttocube(compose_fast(cubetofast(c1), cubetofast(c2)));
}

cube_t
inverse(cube_t cube)
{
	DBG_ASSERT(isconsistent(cube), zero,
	    "inverse error: inconsistent cube\n");

	return fasttocube(inverse_fast(cubetofast(cube)));
}

cube_t
applymoves(cube_t cube, char *buf)
{
	cube_fast_t fast;
	uint8_t r, m;
	char *b;

	DBG_ASSERT(isconsistent(cube), zero,
	    "move error: inconsistent cube\n");

	fast = cubetofast(cube);

	for (b = buf; *b != '\0'; b++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if (*b == '\0')
			goto applymoves_finish;
		if ((r = readmove(*b)) == _error)
			goto applymoves_error;
		if ((m = readmodifier(*(b+1))) != 0)
			b++;
		fast = move(fast, r + m);
	}

applymoves_finish:
	return fasttocube(fast);

applymoves_error:
	DBG_LOG("applymoves error\n");
	return zero;
}

cube_t
applytrans(cube_t cube, char *buf)
{
	cube_fast_t fast;
	uint8_t t;

	DBG_ASSERT(isconsistent(cube), zero,
	    "transformation error: inconsistent cube\n");

	t = readtrans(buf);
	fast = cubetofast(cube);
	fast = transform(fast, t);

	return fasttocube(fast);
}

cube_t
readcube(char *format, char *buf)
{
	cube_t cube;

	if (!strcmp(format, "H48")) {
		cube = readcube_H48(buf);
	} else {
		DBG_LOG("Cannot read cube in the given format\n");
		cube = zero;
	}

	return cube;
}

void
writecube(char *format, cube_t cube, char *buf)
{
	char *errormsg;
	size_t len;

	if (!isconsistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	if (!strcmp(format, "H48")) {
		writecube_H48(cube, buf);
	} else if (!strcmp(format, "SRC")) {
		writecube_SRC(cube, buf);
	} else if (!strcmp(format, "AVX")) {
		writecube_AVX(cube, buf);
	} else {
		errormsg = "ERROR: cannot write cube in the given format";
		goto writecube_error;
	}

	return;

writecube_error:
	DBG_LOG("writecube error, see stdout for details\n");
	len = strlen(errormsg);
	memcpy(buf, errormsg, len);
	buf[len] = '\n';
	buf[len+1] = '\0';
}

_static int
permsign(uint8_t *a, int n)
{
	int i, j;
	uint8_t ret = 0;

	for (i = 0; i < n; i++)
		for (j = i+1; j < n; j++)
			ret += a[i] > a[j] ? 1 : 0;

	return ret % 2;
}

_static uint8_t
readco(char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _ctwist_cw;
	if (*str == '2')
		return _ctwist_ccw;

	DBG_LOG("Error reading CO\n");
	return _error;
}

_static uint8_t
readcp(char *str)
{
	uint8_t c;

	for (c = 0; c < 8; c++)
		if (!strncmp(str, cornerstr[c], 3) ||
		    !strncmp(str, cornerstralt[c], 3))
			return c;

	DBG_LOG("Error reading CP\n");
	return _error;
}

_static uint8_t
readeo(char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _eflip;

	DBG_LOG("Error reading EO\n");
	return _error;
}

_static uint8_t
readep(char *str)
{
	uint8_t e;

	for (e = 0; e < 12; e++)
		if (!strncmp(str, edgestr[e], 2))
			return e;

	DBG_LOG("Error reading EP\n");
	return _error;
}

_static cube_t
readcube_H48(char *buf)
{
	int i;
	uint8_t piece, orient;
	cube_t ret = {0};
	char *b;
	
	b = buf;

	for (i = 0; i < 12; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readep(b)) == _error)
			return zero;
		b += 2;
		if ((orient = readeo(b)) == _error)
			return zero;
		b++;
		ret.edge[i] = piece | orient;
	}
	for (i = 0; i < 8; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readcp(b)) == _error)
			return zero;
		b += 3;
		if ((orient = readco(b)) == _error)
			return zero;
		b++;
		ret.corner[i] = piece | orient;
	}

	return ret;
}

_static int
writepiece_SRC(uint8_t piece, char *buf)
{
	char digits[3];
	int i, len = 0;

	while (piece != 0) {
		digits[len++] = (piece % 10) + '0';
		piece /= 10;
	}

	if (len == 0)
		digits[len++] = '0';

	for (i = 0; i < len; i++)
		buf[i] = digits[len-i-1];

	buf[len] = ',';
	buf[len+1] = ' ';

	return len+2;
}

_static void
writecube_AVX(cube_t cube, char *buf)
{
	int i, ptr;
	uint8_t piece;

	memcpy(buf, "_mm256_set_epi8(\n\t0, 0, 0, 0, ", 30);
	ptr = 30;

	for (i = 11; i >= 0; i--) {
		piece = cube.edge[i];
		ptr += writepiece_SRC(piece, buf + ptr);
	}

	memcpy(buf+ptr-2, ",\n\t0, 0, 0, 0, 0, 0, 0, 0, ", 27);
	ptr += 25;

	for (i = 7; i >= 0; i--) {
		piece = cube.corner[i];
		ptr += writepiece_SRC(piece, buf + ptr);
	}

	memcpy(buf+ptr-2, "\n)\0", 3);
}

_static void
writecube_H48(cube_t cube, char *buf)
{
	uint8_t piece, perm, orient;
	int i;

	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		perm = piece & _pbits;
		orient = (piece & _eobit) >> _eoshift;
		buf[4*i    ] = edgestr[perm][0];
		buf[4*i + 1] = edgestr[perm][1];
		buf[4*i + 2] = orient + '0';
		buf[4*i + 3] = ' ';
	}
	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		perm = piece & _pbits;
		orient = (piece & _cobits) >> _coshift;
		buf[48 + 5*i    ] = cornerstr[perm][0];
		buf[48 + 5*i + 1] = cornerstr[perm][1];
		buf[48 + 5*i + 2] = cornerstr[perm][2];
		buf[48 + 5*i + 3] = orient + '0';
		buf[48 + 5*i + 4] = ' ';
	}

	buf[48+39] = '\0';
}

_static void
writecube_SRC(cube_t cube, char *buf)
{
	int i, ptr;
	uint8_t piece;

	memcpy(buf, "{\n\t.corner = {", 14);
	ptr = 14;

	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		ptr += writepiece_SRC(piece, buf + ptr);
	}

	memcpy(buf+ptr-2, "},\n\t.edge = {", 13);
	ptr += 11;

	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		ptr += writepiece_SRC(piece, buf + ptr);
	}

	memcpy(buf+ptr-2, "}\n}\0", 4);
}

_static uint8_t
readmove(char c)
{
	switch (c) {
	case 'U':
		return U;
	case 'D':
		return D;
	case 'R':
		return R;
	case 'L':
		return L;
	case 'F':
		return F;
	case 'B':
		return B;
	default:
		return _error;
	}
}

_static uint8_t
readmodifier(char c)
{
	switch (c) {
	case '1': /* Fallthrough */
	case '2': /* Fallthrough */
	case '3':
		return c - '0' - 1;
	case '\'':
		return 2;
	default:
		return 0;
	}
}

_static uint8_t
readtrans(char *buf)
{
	uint8_t t;

	for (t = 0; t < 48; t++)
		if (!strncmp(buf, transstr[t], 11))
			return t;

	DBG_LOG("readtrans error\n");
	return _error;
}

_static int
writemoves(uint8_t *m, int n, char *buf)
{
	int i;
	size_t len;
	char *b, *s;

	for (i = 0, b = buf; i < n; i++, b++) {
		s = movestr[m[i]];
		len = strlen(s);
		memcpy(b, s, len);
		b += len;	
		*b = ' ';
	}

	if (b != buf)
		b--; /* Remove last space */
	*b = '\0';

	return b - buf;
}

_static void
writetrans(uint8_t t, char *buf)
{
	if (t >= 48)
		memcpy(buf, "error trans", 11);
	else
		memcpy(buf, transstr[t], 11);
	buf[11] = '\0';
}

_static cube_fast_t
move(cube_fast_t c, uint8_t m)
{
	switch (m) {
	case U:
		return _move(U, c);
	case U2:
		return _move(U2, c);
	case U3:
		return _move(U3, c);
	case D:
		return _move(D, c);
	case D2:
		return _move(D2, c);
	case D3:
		return _move(D3, c);
	case R:
		return _move(R, c);
	case R2:
		return _move(R2, c);
	case R3:
		return _move(R3, c);
	case L:
		return _move(L, c);
	case L2:
		return _move(L2, c);
	case L3:
		return _move(L3, c);
	case F:
		return _move(F, c);
	case F2:
		return _move(F2, c);
	case F3:
		return _move(F3, c);
	case B:
		return _move(B, c);
	case B2:
		return _move(B2, c);
	case B3:
		return _move(B3, c);
	default:
		DBG_LOG("mover error, unknown move\n");
		return zero_fast;
	}
}

_static cube_fast_t
transform(cube_fast_t c, uint8_t t)
{
	switch (t) {
	case UFr:
		return _trans_rotation(UFr, c);
	case ULr:
		return _trans_rotation(ULr, c);
	case UBr:
		return _trans_rotation(UBr, c);
	case URr:
		return _trans_rotation(URr, c);
	case DFr:
		return _trans_rotation(DFr, c);
	case DLr:
		return _trans_rotation(DLr, c);
	case DBr:
		return _trans_rotation(DBr, c);
	case DRr:
		return _trans_rotation(DRr, c);
	case RUr:
		return _trans_rotation(RUr, c);
	case RFr:
		return _trans_rotation(RFr, c);
	case RDr:
		return _trans_rotation(RDr, c);
	case RBr:
		return _trans_rotation(RBr, c);
	case LUr:
		return _trans_rotation(LUr, c);
	case LFr:
		return _trans_rotation(LFr, c);
	case LDr:
		return _trans_rotation(LDr, c);
	case LBr:
		return _trans_rotation(LBr, c);
	case FUr:
		return _trans_rotation(FUr, c);
	case FRr:
		return _trans_rotation(FRr, c);
	case FDr:
		return _trans_rotation(FDr, c);
	case FLr:
		return _trans_rotation(FLr, c);
	case BUr:
		return _trans_rotation(BUr, c);
	case BRr:
		return _trans_rotation(BRr, c);
	case BDr:
		return _trans_rotation(BDr, c);
	case BLr:
		return _trans_rotation(BLr, c);
	case UFm:
		return _trans_mirrored(UFm, c);
	case ULm:
		return _trans_mirrored(ULm, c);
	case UBm:
		return _trans_mirrored(UBm, c);
	case URm:
		return _trans_mirrored(URm, c);
	case DFm:
		return _trans_mirrored(DFm, c);
	case DLm:
		return _trans_mirrored(DLm, c);
	case DBm:
		return _trans_mirrored(DBm, c);
	case DRm:
		return _trans_mirrored(DRm, c);
	case RUm:
		return _trans_mirrored(RUm, c);
	case RFm:
		return _trans_mirrored(RFm, c);
	case RDm:
		return _trans_mirrored(RDm, c);
	case RBm:
		return _trans_mirrored(RBm, c);
	case LUm:
		return _trans_mirrored(LUm, c);
	case LFm:
		return _trans_mirrored(LFm, c);
	case LDm:
		return _trans_mirrored(LDm, c);
	case LBm:
		return _trans_mirrored(LBm, c);
	case FUm:
		return _trans_mirrored(FUm, c);
	case FRm:
		return _trans_mirrored(FRm, c);
	case FDm:
		return _trans_mirrored(FDm, c);
	case FLm:
		return _trans_mirrored(FLm, c);
	case BUm:
		return _trans_mirrored(BUm, c);
	case BRm:
		return _trans_mirrored(BRm, c);
	case BDm:
		return _trans_mirrored(BDm, c);
	case BLm:
		return _trans_mirrored(BLm, c);
	default:
		DBG_LOG("transform error, unknown transformation\n");
		return zero_fast;
	}
}

/******************************************************************************
Section: moves and move sequences

This section contains methods to work with moves and arrays of moves. They
do not rely on the cube structure.
******************************************************************************/

_static_inline uint8_t movebase(uint8_t);
_static_inline uint8_t moveaxis(uint8_t);

_static_inline uint8_t
movebase(uint8_t move)
{
	return move / 3;
}

_static_inline uint8_t
moveaxis(uint8_t move)
{
	return move / 6;
}

/******************************************************************************
Section: solvers

Here you can find the implementation of all the solving algorithms.
******************************************************************************/

typedef struct {
	cube_fast_t cube;
	uint8_t depth;
	int64_t maxsols;
	char **nextsol;
	int64_t *nsols;
	uint8_t nmoves;
	uint8_t moves[20];
	uint8_t (*estimate)(cube_fast_t);
} dfsarg_generic_t;

int64_t solve(cube_t, char *, char *, char *, int8_t, int8_t, int64_t, int8_t,
    void *, char *);
void multisolve(int, cube_t *, char *, void *, char *);
int64_t gendata(char *, void *);

_static bool allowednextmove(uint8_t *, int, uint8_t);
_static void solve_generic_appendsolution(dfsarg_generic_t);
_static int solve_generic_dfs(dfsarg_generic_t);
_static int64_t solve_generic(cube_t, char *, int8_t, int8_t, int64_t, int8_t,
    char *, uint8_t (*)(cube_fast_t));
_static uint8_t estimate_simple(cube_fast_t);
_static int64_t solve_simple(cube_t, int8_t, int8_t, int64_t, int8_t, char *);

int64_t
solve(
	cube_t cube,
	char *solver,
	char *options,
	char *nisstype,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	void *data,
	char *solutions
)
{
	DBG_WARN(!strcmp(options, ""),
	     "solve: 'options' not implemented yet, ignoring\n");

	DBG_WARN(!strcmp(nisstype, ""),
	     "solve: NISS not implemented yet, ignoring 'nisstype'\n");

	DBG_WARN(data == NULL,
	    "solve: 'data' not implemented yet, ignoring\n");

	if (!strcmp(solver, "optimal") || !strcmp(solver, "simple")) {
		return solve_simple(
			cube,
			minmoves,
			maxmoves,
			maxsols,
			optimal,
			solutions
		);
	} else {
		DBG_LOG("solve: unknown solver '%s'\n", solver);
		return -1;
	}

	DBG_LOG("solve: error\n");
	return -1;
}

void
multisolve(int n, cube_t *cube, char *solver, void *data, char *sols)
{
	char *s;
	int i;

	s = sols;
	for (i = 0; i < n; i++) {
		solve(cube[i], solver, "", "normal", 0, -1, 1, 0, NULL, s);
		while (s++);
	}
}

int64_t
gendata(char *solver, void *data)
{
	DBG_LOG("gendata: not implemented yet\n");

	return -1;
}

_static bool
allowednextmove(uint8_t *moves, int n, uint8_t m)
{
	uint8_t mbase, l1base, l2base, maxis, l1axis, l2axis;

	if (n == 0)
		return true;

	mbase = movebase(m);
	maxis = moveaxis(m);
	l1base = movebase(moves[n-1]);
	l1axis = moveaxis(moves[n-1]);

	if (mbase == l1base || (maxis == l1axis && mbase < l1base))
		return false;

	if (n == 1)
		return true;

	l2base = movebase(moves[n-2]);
	l2axis = moveaxis(moves[n-2]);

	return l1axis != l2axis || mbase != l2base;
}

_static void
solve_generic_appendsolution(dfsarg_generic_t arg)
{
	int strl;

	strl = writemoves(arg.moves, arg.depth, *arg.nextsol);
	DBG_LOG("Solution found: %s\n", *arg.nextsol);
	*arg.nextsol += strl;
	**arg.nextsol = '\n';
	(*arg.nextsol)++;
	(*arg.nsols)++;
}

_static int
solve_generic_dfs(dfsarg_generic_t arg)
{
	dfsarg_generic_t nextarg;
	uint8_t m, bound;
	int64_t ret;

	bound = arg.estimate(arg.cube);

	if (*arg.nsols == arg.maxsols || bound + arg.nmoves > arg.depth)
		return 0;

	if (bound == 0) {
		if (arg.nmoves != arg.depth)
			return 0;
		solve_generic_appendsolution(arg);
		return 1;
	}

	memcpy(&nextarg, &arg, sizeof(dfsarg_generic_t));
	nextarg.nmoves = arg.nmoves + 1;
	for (m = 0, ret = 0; m < 18; m++) {
		if (allowednextmove(arg.moves, arg.nmoves, m)) {
			nextarg.cube = move(arg.cube, m);
			nextarg.moves[arg.nmoves] = m;
			ret += solve_generic_dfs(nextarg);
		}
	}

	return ret;
}

_static int64_t
solve_generic(
	cube_t cube,
	char *nisstype,
	/* TODO: handle NISS */
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	char *sols,
	uint8_t (*estimate)(cube_fast_t)
	/* TODO: add validator */
	/* TODO: maybe add data for estimate */
	/* TODO: add moveset (and allowednext?) */
)
{
	dfsarg_generic_t arg;
	int64_t ret, tmp, first;

	if (!issolvable(cube)) {
		DBG_LOG("solve: cube is not solvable\n");
		return -1;
	}

	if (issolved(cube)) {
		DBG_LOG("solve: cube is already solved\n");
		sols[0] = '\n';
		sols[1] = 0;
		return 1;
	}

	DBG_WARN(!strcmp(nisstype, ""),
	    "solve: NISS not implemented yet, 'nisstype' ignored\n");

	if (minmoves < 0) {
		DBG_LOG("solve: 'minmoves' is negative, setting to 0\n");
		minmoves = 0;
	}

	if (maxmoves < 0) {
		DBG_LOG("solve: invalid 'maxmoves', setting to 20\n");
		maxmoves = 20;
	}

	if (maxsols < 0) {
		DBG_LOG("solve: 'maxsols' is negative\n");
		return -1;
	}

	if (maxsols == 0) {
		DBG_LOG("solve: 'maxsols' is 0\n");
		return 0;
	}

	if (sols == NULL) {
		DBG_LOG("solve: return parameter 'sols' is NULL\n");
		return -1;
	}

	if (estimate == NULL) {
		DBG_LOG("solve: 'estimate' is NULL\n");
		return -1;
	}

	arg = (dfsarg_generic_t) {
		.cube = cubetofast(cube),
		.maxsols = maxsols,
		.nextsol = &sols,
		.nsols = &ret,
		.nmoves = 0,
		.moves = {0},
		.estimate = estimate,
	};

	ret = 0;
	first = -1;
	for (arg.depth = minmoves; arg.depth <= maxmoves; arg.depth++) {
		tmp = solve_generic_dfs(arg);
		if (tmp != 0)
			first = arg.depth;

		DBG_LOG("Found %" PRId64 " solution%s at depth %" PRIu8 "\n",
		        tmp, tmp == 1 ? "" : "s", arg.depth);

		if (ret >= maxsols)
			break;

		if (optimal >= 0 && first >= 0 && arg.depth - first == optimal)
			break;
	}

	DBG_ASSERT(ret <= maxsols, ret,
	    "solve: found more than 'maxsols' solutions\n");

	return ret;
}

_static uint8_t
estimate_simple(cube_fast_t cube)
{
	return issolved_fast(cube) ? 0 : 1;
}

_static int64_t
solve_simple(
	cube_t cube,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	char *solutions
)
{
	return solve_generic(
		cube,
		"",
		minmoves,
		maxmoves,
		maxsols,
		optimal,
		solutions,
		&estimate_simple
	);
}
