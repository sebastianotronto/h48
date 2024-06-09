/* TODO: change cube to const char [static 22], or const char * */
/* TODO: return error code */
/* TODO: implement all of this, or move the implementation to cube_public.h */
/* TODO: document in a separate file, possibly leave here 1-line comments */

cube_t compose(cube_t cube, cube_t perm);
cube_t inverse(cube_t cube);
cube_t solvedcube(void);
cube_t applymoves(cube_t, const char *);
cube_t applytrans(cube_t, const char *);
cube_t readcube(const char *format, const char *buf);
void writecube(const char *format, cube_t cube, char *buf);
int64_t solve(
	cube_t cube,
	const char *solver, 
	const char *options,
	const char *nisstype,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	const void *data,         
	char *solutions
);
void multisolve(
	int n,
	cube_t *cube,
	const char *solver,
	const void *data,
	char *sols
);
int64_t gendata(const char *solver, const char *options, void *data);
