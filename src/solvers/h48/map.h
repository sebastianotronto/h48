#define MAP_UNSET             UINT64_C(0xFFFFFFFFFFFFFFFF)
#define MAP_KEYMASK           UINT64_C(0xFFFFFFFFFF)
#define MAP_KEYSHIFT          UINT64_C(40)

typedef struct {
	uint64_t n;
	uint64_t capacity;
	uint64_t randomizer;
	uint64_t *table;
} h48map_t;

typedef struct {
	uint64_t key;
	uint64_t val;
} kvpair_t;

_static void h48map_create(h48map_t *, uint64_t, uint64_t);
_static void h48map_clear(h48map_t *);
_static void h48map_destroy(h48map_t *);
_static uint64_t h48map_lookup(h48map_t *, uint64_t);
_static void h48map_insertmin(h48map_t *, uint64_t, uint64_t);
_static uint64_t h48map_value(h48map_t *, uint64_t);
_static kvpair_t h48map_nextkvpair(h48map_t *, uint64_t *);

_static void
h48map_create(h48map_t *map, uint64_t capacity, uint64_t randomizer)
{
	map->capacity = capacity;
	map->randomizer = randomizer;

	map->table = malloc(map->capacity * sizeof(int64_t));
	h48map_clear(map);
}

_static void
h48map_clear(h48map_t *map)
{
	memset(map->table, 0xFF, map->capacity * sizeof(uint64_t));
	map->n = 0;
}

_static void
h48map_destroy(h48map_t *map)
{
	free(map->table);
}

_static_inline uint64_t
h48map_lookup(h48map_t *map, uint64_t x)
{
	uint64_t hash, i;

	hash = ((x % map->capacity) * map->randomizer) % map->capacity;
	for (i = hash;
	     map->table[i] != MAP_UNSET && (map->table[i] & MAP_KEYMASK) != x;
	     i = (i+1) % map->capacity
	) ;

	return i;
}

_static_inline void
h48map_insertmin(h48map_t *map, uint64_t key, uint64_t val)
{
	uint64_t i, oldval, min;

	i = h48map_lookup(map, key);
	oldval = map->table[i] >> MAP_KEYSHIFT;
	min = _min(val, oldval);

	map->n += map->table[i] == MAP_UNSET;
	map->table[i] = (key & MAP_KEYMASK) | (min << MAP_KEYSHIFT);
}

_static_inline uint64_t
h48map_value(h48map_t *map, uint64_t key)
{
	return map->table[h48map_lookup(map, key)] >> MAP_KEYSHIFT;
}

_static kvpair_t
h48map_nextkvpair(h48map_t *map, uint64_t *p)
{
	kvpair_t kv;
	uint64_t pair;

	kv.key = MAP_UNSET;
	kv.val = MAP_UNSET;

	DBG_ASSERT(*p < map->capacity, kv,
	    "Error looping over map: given index %" PRIu64 " is out of "
	    "range [0,%" PRIu64 "]", *p, map->capacity);

	for ( ; *p < map->capacity; (*p)++) {
		if (map->table[*p] != MAP_UNSET) {
			pair = map->table[(*p)++];
			kv.key = pair & MAP_KEYMASK;
			kv.val = pair >> MAP_KEYSHIFT;
			return kv;
		}
	}

	return kv;
}
