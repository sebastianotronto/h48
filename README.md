# Prototype for a new optimal solver

Work in progress.

## Running tests

```
$ ./configure.sh # Run 'TYPE=AVX2 ./configure.sh' to use AVX2 instead
$ make test
```

## TODO:

### AVX2

* static `solvedcube` and co don't work, turn into functions?
* implement missing stuff (moves, transform)
* optimize things that use _base functions

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
