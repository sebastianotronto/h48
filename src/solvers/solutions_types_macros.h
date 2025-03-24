#define MAXLEN 20

typedef struct {
	uint8_t nmoves;
	uint8_t moves[MAXLEN];
	uint8_t npremoves;
	uint8_t premoves[MAXLEN];
} solution_moves_t;

typedef struct {
	uint64_t tmask;
	bool unniss;
	uint8_t maxmoves;
	uint64_t maxsolutions;
	int8_t optimal;
} solution_settings_t;

typedef struct {
	_Atomic uint64_t nsols;
	uint8_t shortest_sol;
	size_t size;
	size_t used;
	char *buf;
} solution_list_t;
