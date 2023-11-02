typedef struct {
	uint8_t c[16];
	uint8_t e[16];
} cube_arr_t;
#ifdef CUBE_AVX2
typedef __m256i cube_t;
#else
typedef cube_arr_t cube_t;
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
