/* TODO: ifdef for different implementations */
#include "cube_array.h"

/* For moves and transformations, see constants.h */
typedef uint8_t move_t;
typedef uint8_t trans_t;

extern cube_t solvedcube;

/* For the textual representation of the cube, see utils/FORMAT.txt */
cube_t readcube(char *);
void writecube(cube_t, char *);

/* Writes a cube in C source code format */
void writecubesrc(cube_t, char *);

bool issolvable(cube_t);
bool equal(cube_t, cube_t);
bool issolved(cube_t);
bool iserror(cube_t);

int readmoves(char *, move_t *);
void writemoves(move_t *, int, char *);

cube_t move(cube_t, move_t);
cube_t inverse(cube_t);
cube_t compose(cube_t, cube_t);
cube_t transform(cube_t, trans_t);
