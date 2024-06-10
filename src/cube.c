#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#define _static
#define _static_inline
#define DBG_LOG(...) fprintf(stderr, __VA_ARGS__)
#define DBG_WARN(condition, ...) if (!(condition)) DBG_LOG(__VA_ARGS__);
#define DBG_ASSERT(condition, retval, ...) \
    if (!(condition)) { DBG_LOG(__VA_ARGS__); return retval; }

#else
#define _static static
#define _static_inline static inline
#define DBG_LOG(...)
#define DBG_WARN(condition, ...)
#define DBG_ASSERT(condition, retval, ...)
#endif

#include "constants.h"

#if defined(CUBE_AVX2)
#include <immintrin.h>
#include "cube_avx2.h"
#elif defined(CUBE_NEON)
#include "cube_neon.h"
#else
#include <stdlib.h> /* TODO: check if can be removed */
#include "cube_portable.h"
#endif

#include "io_move_trans.h"
#include "constant_cubes.h"
#include "cube_generic.h"
#include "io_cube.h"

/* TODO: work in progress */
#if 0
#include "constant_cubes_transform.h"
#include "cube_transform.h"
#else
#include "cube_transform_with_switch.h"
#endif

#include "moves.h"
#include "solve_h48.h"
#include "solve_generic.h"

#include "cube_public.h"
