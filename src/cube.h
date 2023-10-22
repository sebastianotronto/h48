/* Constants *****************************************************************/

/* Standard moves */
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

/* Regular transformations (rotations) */
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

/* Mirrored transformations */
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

/* Errors */
#define errormove 99U
#define errortrans 99U

/* Types *********************************************************************/

/* TODO: ifdef for different implementations */
/* See doc/CUBE_INTERNAL.md for a description of the cube format */
typedef struct {
	uint8_t c[8];
	uint8_t e[12];
} cube_t;

typedef uint8_t move_t;
typedef uint8_t trans_t;

extern cube_t solvedcube;

/* Functions *****************************************************************/

/* Not all formats are supported for both read and write. */
/* See doc/CUBE_TEXT.md for details. */
typedef enum {H48, SRC} format_t;
cube_t readcube(format_t, char *);
void writecube(format_t, cube_t, char *);

bool issolvable(cube_t);
bool equal(cube_t, cube_t);
bool issolved(cube_t);
bool iserror(cube_t);

int readmoves(char *, move_t *);
void writemoves(move_t *, int, char *);

trans_t readtrans(char *);
void writetrans(trans_t, char *);

cube_t move(cube_t, move_t);
cube_t inverse(cube_t);
cube_t compose(cube_t, cube_t);

/* See doc/TRANSFORMATIONS.md for how transformations are applied */
cube_t transform(cube_t, trans_t);
