#include "../cube.h"

#ifdef CUBE_AVX2
typedef __m256i cube_fast_t;
#else
typedef cube_t cube_fast_t;
#endif
