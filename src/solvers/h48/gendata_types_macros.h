#define COCSEP_CLASSES      ((size_t)3393)
#define COCSEP_TABLESIZE    ((size_t)POW_3_7 << (size_t)7)
#define COCSEP_VISITEDSIZE  DIV_ROUND_UP(COCSEP_TABLESIZE, (size_t)8)
#define COCSEP_FULLSIZE     (INFOSIZE + (size_t)4 * COCSEP_TABLESIZE)

#define VISITED_IND(i)      ((uint32_t)(i) / UINT32_C(8))
#define VISITED_MASK(i)     (UINT32_C(1) << ((uint32_t)(i) % UINT32_C(8)))

#define CBOUND_MASK         UINT32_C(0xFF)
#define CBOUND(x)           ((x) & CBOUND_MASK)

#define H48_COORDMAX_NOEO   ((int64_t)(COCSEP_CLASSES * COMB_12_4 * COMB_8_4))
#define H48_COORDMAX(h)     (H48_COORDMAX_NOEO << (int64_t)(h))
#define H48_DIV(k)          ((size_t)8 / (size_t)(k))
#define H48_TABLESIZE(h, k) DIV_ROUND_UP((size_t)H48_COORDMAX((h)), H48_DIV(k))

#define H48_COEFF(k)        (INT64_C(8) / (int64_t)(k))
#define H48_INDEX(i, k)     ((i) / H48_COEFF(k))
#define H48_SHIFT(i, k)     ((uint8_t)(k) * (uint8_t)((i) % H48_COEFF(k)))
#define H48_MASK(i, k)      ((UINT8_BIT(k) - UINT8_C(1)) << H48_SHIFT(i, k))

#define MAXLEN 20
#define CHUNKS COCSEP_CLASSES

typedef struct {
	cube_t cube;
	uint8_t depth;
	uint8_t maxdepth;
	uint16_t *n;
	uint32_t *buf32;
	uint8_t *visited;
	uint64_t *selfsim;
	cube_t *rep;
} cocsep_dfs_arg_t;

typedef struct {
	uint8_t h;
	uint8_t k;
	uint8_t base;
	uint8_t maxdepth;
	tableinfo_t info;
	void *buf;
	void *h48buf;
	uint32_t *cocsepdata;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t crep[COCSEP_CLASSES];
} gendata_h48_arg_t;

typedef struct {
	uint8_t maxdepth;
	const uint32_t *cocsepdata;
	const cube_t *crep;
	const uint64_t *selfsim;
	h48map_t *map;
} gendata_h48short_arg_t;

typedef struct {
	uint8_t depth;
	uint32_t *cocsepdata;
	_Atomic uint8_t *table;
	uint64_t *selfsim;
	cube_t *crep;
	uint64_t start;
	uint64_t end;
	pthread_mutex_t *table_mutex[CHUNKS];
} h48h0k4_bfs_arg_t;

typedef struct {
	cube_t cube;
	uint8_t h;
	uint8_t k;
	uint8_t base;
	uint8_t shortdepth;
	uint32_t *cocsepdata;
	_Atomic uint8_t *table;
	uint64_t *selfsim;
	cube_t *crep;
	h48map_t *shortcubes;
	pthread_mutex_t *shortcubes_mutex;
	pthread_mutex_t *table_mutex[CHUNKS];
	uint64_t *next;
	uint64_t *count;
} h48k2_dfs_arg_t;

typedef struct {
	cube_t cube;
	int8_t depth;
	uint8_t h;
	uint8_t k;
	uint32_t *cocsepdata;
	uint64_t *selfsim;
	_Atomic uint8_t *table;
	pthread_mutex_t **table_mutex;
} gendata_h48_mark_t;
