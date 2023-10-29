# Prototype for a new optimal solver

Work in progress.

## Running tests

```
$ ./configure.sh # Run 'TYPE=AVX2 ./configure.sh' to use AVX2 instead
$ make test
```

## TODO:

### Tests

* inverse_move
* inverse_trans

### AVX2

* fix base get_ and set_ macros (constant arguments?)
* implement missing stuff (moves, transform)
* optimize things that use get_ and set_

### More features

* coordinates: co, eo, epsep, cpsep_sym, cocpsep_sym, cphtr_sym, cocphtr_sym
* pruning tables (1 bit per entry + fallback)
* solve.c

### Optimizations:

* multi-move (up to 4/5 moves at once)

### Things I need to learn:

* Use AVX2 instructions, in particular
  [_mm256_shuffle_epi8](https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-10/mm256-shuffle-epi8.html))
* Inspect compiled assembly
* Use valgrind tool cachegrind and other profiling tools
