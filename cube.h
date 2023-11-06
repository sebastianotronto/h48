#ifdef CUBE_AVX2
#include <immintrin.h>
typedef __m256i cube_t;
#else
typedef struct {
	uint8_t c[16]; /* Only the first 8 are used, the rest is padding */
	uint8_t e[16]; /* Only the first 12 are used, the rest is padding */
} cube_t;
#endif

typedef uint8_t move_t;
typedef uint8_t trans_t;

int readmoves(char *, move_t *);
void writemoves(move_t *, int, char *);
trans_t readtrans(char *);
void writetrans(trans_t, char *);

typedef enum {AVX, H48, SRC} format_t;
cube_t readcube(format_t, char *);        /* Supports: H48 */
void writecube(format_t, cube_t, char *); /* Supports: AVX, H48, SRC */

cube_t solvedcube(void);
cube_t zerocube(void);
bool issolvable(cube_t);
bool equal(cube_t, cube_t);
bool issolved(cube_t);
bool iserror(cube_t);

cube_t move(cube_t, move_t);
cube_t inverse(cube_t);
cube_t compose(cube_t, cube_t);
cube_t transform(cube_t, trans_t);

int16_t coord_eo(cube_t);

/* Solvers return -1 in case of error, the number of solutions otherwise */
int solve_small(cube_t, uint8_t, int, move_t *);
