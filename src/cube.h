/* TODO: ifdef for different implementations */
#include "cube_array.h"

/* For moves and transformations, see constants.h */
typedef uint8_t move_t;
typedef uint8_t trans_t;

extern cube_t solvedcube;

/* Not all formats are supported for both read and write. */
/* See utils/FORMAT.txt for details. */
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

/* See utils/TRANSFORMATIONS.txt for how transformations are applied */
cube_t transform(cube_t, trans_t);
