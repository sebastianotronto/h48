/******************************************************************************
Section: benchmarks

Here you can find some simple functions that can be used to benchmark the
rest of the code.
******************************************************************************/

#define RANDOMCUBES 157

static void
setup_randomcubes(cube_fast_t *cubes)
{
	int i;

	for (i = 0; i < RANDOMCUBES; i++)
		cubes[i] = run_moves(i*4);
}

cube_t
run_moves(int64_t n)
{
	cube_fast_t fast;
	int64_t m, i;

	fast = solvedcube();
	m = n / 18;

	for (i = 0; i < m; i++) {
		fast = _move_U(fast);
		fast = _move_U2(fast);
		fast = _move_U3(fast);
		fast = _move_D(fast);
		fast = _move_D2(fast);
		fast = _move_D3(fast);
		fast = _move_R(fast);
		fast = _move_R2(fast);
		fast = _move_R3(fast);
		fast = _move_L(fast);
		fast = _move_L2(fast);
		fast = _move_L3(fast);
		fast = _move_F(fast);
		fast = _move_F2(fast);
		fast = _move_F3(fast);
		fast = _move_B(fast);
		fast = _move_B2(fast);
		fast = _move_B3(fast);
	}

	for (i = m * 18; i < n; i++)
		fast = _move_F(fast);

	return fast;
}

cube_t
run_trans(int64_t n)
{
	cube_fast_t fast;
	int64_t m, i;

	fast = run_moves(33);
	m = n / 18;

	for (i = 0; i < m; i++) {
		fast = _trans_UFr(fast);
		fast = _trans_ULr(fast);
		fast = _trans_UBr(fast);
		fast = _trans_URr(fast);
		fast = _trans_DFr(fast);
		fast = _trans_DLr(fast);
		fast = _trans_DBr(fast);
		fast = _trans_DRr(fast);
		fast = _trans_RUr(fast);
		fast = _trans_RFr(fast);
		fast = _trans_RDr(fast);
		fast = _trans_RBr(fast);
		fast = _trans_LUr(fast);
		fast = _trans_LFr(fast);
		fast = _trans_LDr(fast);
		fast = _trans_LBr(fast);
		fast = _trans_FUr(fast);
		fast = _trans_FRr(fast);
		fast = _trans_FDr(fast);
		fast = _trans_FLr(fast);
		fast = _trans_BUr(fast);
		fast = _trans_BRr(fast);
		fast = _trans_BDr(fast);
		fast = _trans_BLr(fast);
		fast = _trans_UFm(fast);
		fast = _trans_ULm(fast);
		fast = _trans_UBm(fast);
		fast = _trans_URm(fast);
		fast = _trans_DFm(fast);
		fast = _trans_DLm(fast);
		fast = _trans_DBm(fast);
		fast = _trans_DRm(fast);
		fast = _trans_RUm(fast);
		fast = _trans_RFm(fast);
		fast = _trans_RDm(fast);
		fast = _trans_RBm(fast);
		fast = _trans_LUm(fast);
		fast = _trans_LFm(fast);
		fast = _trans_LDm(fast);
		fast = _trans_LBm(fast);
		fast = _trans_FUm(fast);
		fast = _trans_FRm(fast);
		fast = _trans_FDm(fast);
		fast = _trans_FLm(fast);
		fast = _trans_BUm(fast);
		fast = _trans_BRm(fast);
		fast = _trans_BDm(fast);
		fast = _trans_BLm(fast);
	}

	for (i = m * 18; i < n; i++)
		fast = _trans_FRm(fast);

	return fast;
}

cube_t
run_compose(int64_t n)
{
	cube_fast_t fast, cubes[RANDOMCUBES];
	int64_t i;

	setup_randomcubes(cubes);

	for (i = 0; i < n; i++)
		fast = compose_fast(fast, cubes[i % RANDOMCUBES]);

	return fast;
}

cube_t
run_inverse(int64_t n)
{
	cube_fast_t fast, cubes[RANDOMCUBES];
	int64_t i;

	setup_randomcubes(cubes);

	for (i = 0; i < n; i++)
		fast = inverse_fast(cubes[i % RANDOMCUBES]);

	return fast;
}
