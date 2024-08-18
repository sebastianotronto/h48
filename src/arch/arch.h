#if defined(CUBE_AVX2)

#include <immintrin.h>

typedef __m256i cube_t;

#if !defined(TEST_H)
#include "common.h"
#include "avx2.h"
#endif

#elif defined(CUBE_NEON)

#include <stdlib.h>
#include <arm_neon.h>

typedef struct {
	uint8x16_t corner;
	uint8x16_t edge;
} cube_t;

#if !defined(TEST_H)
#include "common.h"
#include "neon.h"
#endif

#else

#include <stdlib.h>

typedef struct {
	uint8_t corner[8];
	uint8_t edge[12];
} cube_t;

#if !defined(TEST_H)
#include "common.h"
#include "portable.h"
#endif

#endif
