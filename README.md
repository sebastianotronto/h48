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

### Simple additions

* Write to AVX2-src format
* move() that takes a string (alg) as input

### Changes

* write one function (static inline or public?) for each move (and trans)
* only call the specific functions in performance-critical steps (i.e. solve
  - if the functions are static inline, all performance-critical steps must
  be internal to cube.c)
* this also enables skipping some unnecessary work (e.g. flip edges, twist
  corners) for many moves (and mirror transformations)
* add benchmarks with moves / trans called directly instead of via the
  generic function
* keep generic move and trans functions with big switch case
* bring back constants into cube.c, and maybe also moves (TBD: what to
  do with architecture-specific code? leave in separate files like now,
  use just one file for each architecture...)

### Documentation and interface

* remove the constant #define's from cube.h (write a comment instead)
* reconsider content of cube.h, remove some stuff
* remove doc folder, inline documentation as comments in cube.h or cube.c

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
