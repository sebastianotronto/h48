#define COORD_INDEX(i) ((i)/2)
#define COORD_SHIFT(i) (UINT8_C(4) * (uint8_t)((i) % 2))
#define COORD_MASK(i)  (UINT8_C(0xF) << COORD_SHIFT(i))

typedef struct {
	const char name[255];
	uint64_t (*coord)(cube_t, const void *);
	cube_t (*cube)(uint64_t, const void *);
	size_t (*gendata)(void *);
	uint64_t max;
	uint32_t moves_mask;
	uint64_t trans_mask;
	uint8_t axistrans[3];
} coord_t;
