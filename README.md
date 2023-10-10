# Prototype for a new optimal solver

Work in progress.

TODO:

* test SRC: scrambled cube, rename directory in io_SRC_write
* implement transformations (use write to SRC to make life easier)
* setup benchmarks
* coordinates: co, eo, epsep, cpsep_sym, cocpsep_sym, cphtr_sym, cocphtr_sym
* pruning tables (1 bit per entry + fallback)
* solve.c

Optimizations:

* avx2_cube.c
* multi-move (up to 4/5 moves at once)

Things I need to learn:

* Use AVX2 instructions, in particular
  [_mm256_shuffle_epi8](https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-10/mm256-shuffle-epi8.html))
* Inspect compiled assembly
* Use valgrind tool cachegrind and other profiling tools
