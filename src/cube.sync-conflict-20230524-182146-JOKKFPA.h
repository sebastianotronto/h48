typedef enum {
        U, U2, U3, D, D2, D3,
        R, R2, R3, L, L2, L3,
        F, F2, F3, B, B2, B3
} move_t;
typedef struct {
	uint64_t e;
	uint64_t c;
} cube_t;

extern cube_t solvedcube;
extern cube_t errorcube;

bool isconsistent(cube_t);
bool issolved(cube_t);

cube_t readcube(char *);
void writecube(cube_t, char *);

int readmoves(char *, move_t *);
void writemoves(move_t *, int, char *);

/*
cube_t move(move_t, cube_t);
cube_t inverse(cube_t);

*/
