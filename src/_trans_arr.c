static inline cube_t
flipallcorners(cube_t c)
{
	uint8_t i, piece, orien;
	cube_t ret;

	ret = c;
	for (i = 0; i < 8; i++) {
		piece = get_corner(c, i);
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		set_corner(ret, i, (piece & _pbits) | orien);
	}

	return ret;
}

static inline cube_t
inline_trans_UFr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {0, 1, 2, 3, 4, 5, 6, 7},
		.e = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
	};
	cube_t ti = {
		.c = {0, 1, 2, 3, 4, 5, 6, 7},
		.e = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_ULr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {4, 5, 7, 6, 1, 0, 2, 3},
		.e = {5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24}
	};
	cube_t ti = {
		.c = {5, 4, 6, 7, 0, 1, 3, 2},
		.e = {4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_UBr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {1, 0, 3, 2, 5, 4, 7, 6},
		.e = {1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9}
	};
	cube_t ti = {
		.c = {1, 0, 3, 2, 5, 4, 7, 6},
		.e = {1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_URr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {5, 4, 6, 7, 0, 1, 3, 2},
		.e = {4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26}
	};
	cube_t ti = {
		.c = {4, 5, 7, 6, 1, 0, 2, 3},
		.e = {5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_DFr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {2, 3, 0, 1, 6, 7, 4, 5},
		.e = {3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10}
	};
	cube_t ti = {
		.c = {2, 3, 0, 1, 6, 7, 4, 5},
		.e = {3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_DLr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {7, 6, 4, 5, 2, 3, 1, 0},
		.e = {6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27}
	};
	cube_t ti = {
		.c = {7, 6, 4, 5, 2, 3, 1, 0},
		.e = {6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_DBr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {3, 2, 1, 0, 7, 6, 5, 4},
		.e = {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8}
	};
	cube_t ti = {
		.c = {3, 2, 1, 0, 7, 6, 5, 4},
		.e = {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_DRr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {6, 7, 5, 4, 3, 2, 0, 1},
		.e = {7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25}
	};
	cube_t ti = {
		.c = {6, 7, 5, 4, 3, 2, 0, 1},
		.e = {7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_RUr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {64, 67, 65, 66, 37, 38, 36, 39},
		.e = {20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3}
	};
	cube_t ti = {
		.c = {32, 34, 35, 33, 70, 68, 69, 71},
		.e = {8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_RFr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {38, 37, 36, 39, 64, 67, 66, 65},
		.e = {24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18}
	};
	cube_t ti = {
		.c = {36, 39, 38, 37, 66, 65, 64, 67},
		.e = {25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_RDr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {67, 64, 66, 65, 38, 37, 39, 36},
		.e = {23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1}
	};
	cube_t ti = {
		.c = {33, 35, 34, 32, 71, 69, 68, 70},
		.e = {10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_RBr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {37, 38, 39, 36, 67, 64, 65, 66},
		.e = {27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16}
	};
	cube_t ti = {
		.c = {37, 38, 39, 36, 67, 64, 65, 66},
		.e = {27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_LUr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {65, 66, 64, 67, 36, 39, 37, 38},
		.e = {21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2}
	};
	cube_t ti = {
		.c = {34, 32, 33, 35, 68, 70, 71, 69},
		.e = {9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_LFr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {36, 39, 38, 37, 66, 65, 64, 67},
		.e = {25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17}
	};
	cube_t ti = {
		.c = {38, 37, 36, 39, 64, 67, 66, 65},
		.e = {24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_LDr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {66, 65, 67, 64, 39, 36, 38, 37},
		.e = {22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0}
	};
	cube_t ti = {
		.c = {35, 33, 32, 34, 69, 71, 70, 68},
		.e = {11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_LBr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {39, 36, 37, 38, 65, 66, 67, 64},
		.e = {26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19}
	};
	cube_t ti = {
		.c = {39, 36, 37, 38, 65, 66, 67, 64},
		.e = {26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_FUr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {68, 70, 69, 71, 32, 34, 33, 35},
		.e = {16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6}
	};
	cube_t ti = {
		.c = {68, 70, 69, 71, 32, 34, 33, 35},
		.e = {16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_FRr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {32, 34, 35, 33, 70, 68, 69, 71},
		.e = {8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21}
	};
	cube_t ti = {
		.c = {64, 67, 65, 66, 37, 38, 36, 39},
		.e = {20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_FDr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {70, 68, 71, 69, 34, 32, 35, 33},
		.e = {19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4}
	};
	cube_t ti = {
		.c = {69, 71, 68, 70, 33, 35, 32, 34},
		.e = {17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_FLr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {34, 32, 33, 35, 68, 70, 71, 69},
		.e = {9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23}
	};
	cube_t ti = {
		.c = {65, 66, 64, 67, 36, 39, 37, 38},
		.e = {21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_BUr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {69, 71, 68, 70, 33, 35, 32, 34},
		.e = {17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7}
	};
	cube_t ti = {
		.c = {70, 68, 71, 69, 34, 32, 35, 33},
		.e = {19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_BRr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {35, 33, 32, 34, 69, 71, 70, 68},
		.e = {11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22}
	};
	cube_t ti = {
		.c = {66, 65, 67, 64, 39, 36, 38, 37},
		.e = {22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_BDr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {71, 69, 70, 68, 35, 33, 34, 32},
		.e = {18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5}
	};
	cube_t ti = {
		.c = {71, 69, 70, 68, 35, 33, 34, 32},
		.e = {18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_BLr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {33, 35, 34, 32, 71, 69, 68, 70},
		.e = {10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20}
	};
	cube_t ti = {
		.c = {67, 64, 66, 65, 38, 37, 39, 36},
		.e = {23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_UFm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {4, 5, 6, 7, 0, 1, 2, 3},
		.e = {0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10}
	};
	cube_t ti = {
		.c = {4, 5, 6, 7, 0, 1, 2, 3},
		.e = {0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_ULm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {0, 1, 3, 2, 5, 4, 6, 7},
		.e = {4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25}
	};
	cube_t ti = {
		.c = {0, 1, 3, 2, 5, 4, 6, 7},
		.e = {4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_UBm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {5, 4, 7, 6, 1, 0, 3, 2},
		.e = {1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8}
	};
	cube_t ti = {
		.c = {5, 4, 7, 6, 1, 0, 3, 2},
		.e = {1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_URm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {1, 0, 2, 3, 4, 5, 7, 6},
		.e = {5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27}
	};
	cube_t ti = {
		.c = {1, 0, 2, 3, 4, 5, 7, 6},
		.e = {5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_DFm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {6, 7, 4, 5, 2, 3, 0, 1},
		.e = {3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11}
	};
	cube_t ti = {
		.c = {6, 7, 4, 5, 2, 3, 0, 1},
		.e = {3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_DLm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {3, 2, 0, 1, 6, 7, 5, 4},
		.e = {7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26}
	};
	cube_t ti = {
		.c = {2, 3, 1, 0, 7, 6, 4, 5},
		.e = {6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_DBm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {7, 6, 5, 4, 3, 2, 1, 0},
		.e = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9}
	};
	cube_t ti = {
		.c = {7, 6, 5, 4, 3, 2, 1, 0},
		.e = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_DRm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {2, 3, 1, 0, 7, 6, 4, 5},
		.e = {6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24}
	};
	cube_t ti = {
		.c = {3, 2, 0, 1, 6, 7, 5, 4},
		.e = {7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_RUm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {68, 71, 69, 70, 33, 34, 32, 35},
		.e = {21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3}
	};
	cube_t ti = {
		.c = {70, 68, 69, 71, 32, 34, 35, 33},
		.e = {8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_RFm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {34, 33, 32, 35, 68, 71, 70, 69},
		.e = {25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18}
	};
	cube_t ti = {
		.c = {66, 65, 64, 67, 36, 39, 38, 37},
		.e = {25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_RDm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {71, 68, 70, 69, 34, 33, 35, 32},
		.e = {22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1}
	};
	cube_t ti = {
		.c = {71, 69, 68, 70, 33, 35, 34, 32},
		.e = {10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_RBm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {33, 34, 35, 32, 71, 68, 69, 70},
		.e = {26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16}
	};
	cube_t ti = {
		.c = {67, 64, 65, 66, 37, 38, 39, 36},
		.e = {27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_LUm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {69, 70, 68, 71, 32, 35, 33, 34},
		.e = {20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2}
	};
	cube_t ti = {
		.c = {68, 70, 71, 69, 34, 32, 33, 35},
		.e = {9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_LFm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {32, 35, 34, 33, 70, 69, 68, 71},
		.e = {24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17}
	};
	cube_t ti = {
		.c = {64, 67, 66, 65, 38, 37, 36, 39},
		.e = {24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_LDm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {70, 69, 71, 68, 35, 32, 34, 33},
		.e = {23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0}
	};
	cube_t ti = {
		.c = {69, 71, 70, 68, 35, 33, 32, 34},
		.e = {11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_LBm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {35, 32, 33, 34, 69, 70, 71, 68},
		.e = {27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19}
	};
	cube_t ti = {
		.c = {65, 66, 67, 64, 39, 36, 37, 38},
		.e = {26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_FUm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {64, 66, 65, 67, 36, 38, 37, 39},
		.e = {16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7}
	};
	cube_t ti = {
		.c = {32, 34, 33, 35, 68, 70, 69, 71},
		.e = {16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_FRm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {36, 38, 39, 37, 66, 64, 65, 67},
		.e = {9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20}
	};
	cube_t ti = {
		.c = {37, 38, 36, 39, 64, 67, 65, 66},
		.e = {20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_FDm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {66, 64, 67, 65, 38, 36, 39, 37},
		.e = {19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5}
	};
	cube_t ti = {
		.c = {33, 35, 32, 34, 69, 71, 68, 70},
		.e = {17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_FLm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {38, 36, 37, 39, 64, 66, 67, 65},
		.e = {8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22}
	};
	cube_t ti = {
		.c = {36, 39, 37, 38, 65, 66, 64, 67},
		.e = {21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_BUm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {65, 67, 64, 66, 37, 39, 36, 38},
		.e = {17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6}
	};
	cube_t ti = {
		.c = {34, 32, 35, 33, 70, 68, 71, 69},
		.e = {19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_BRm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {39, 37, 36, 38, 65, 67, 66, 64},
		.e = {10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23}
	};
	cube_t ti = {
		.c = {39, 36, 38, 37, 66, 65, 67, 64},
		.e = {22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_BDm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {67, 65, 66, 64, 39, 37, 38, 36},
		.e = {18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4}
	};
	cube_t ti = {
		.c = {35, 33, 34, 32, 71, 69, 70, 68},
		.e = {18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_BLm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {37, 39, 38, 36, 67, 65, 64, 66},
		.e = {11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21}
	};
	cube_t ti = {
		.c = {38, 37, 39, 36, 67, 64, 66, 65},
		.e = {23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

