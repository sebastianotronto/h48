_static void pieces(cube_t *, uint8_t [static 8], uint8_t [static 12]);
_static_inline bool equal(cube_t, cube_t);
_static_inline cube_t invertco(cube_t);
_static_inline cube_t compose_epcpeo(cube_t, cube_t);
_static_inline cube_t compose_edges(cube_t, cube_t);
_static_inline cube_t compose_corners(cube_t, cube_t);
_static_inline cube_t compose(cube_t, cube_t);
_static_inline cube_t inverse(cube_t);

_static_inline int64_t coord_co(cube_t);
_static_inline int64_t coord_csep(cube_t);
_static_inline int64_t coord_cocsep(cube_t);
_static_inline int64_t coord_eo(cube_t);
_static_inline int64_t coord_esep(cube_t);

_static_inline void copy_corners(cube_t *, cube_t);
_static_inline void copy_edges(cube_t *, cube_t);
_static_inline void set_eo(cube_t *, int64_t);
_static_inline cube_t invcoord_esep(int64_t);
