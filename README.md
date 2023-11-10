# Prototype for a new optimal solver

Work in progress. Everything is in a state of flux and can change without
notice.

## Building and running tests

First run

```
$ ./configure.sh
```

or `TYPE=AVX2 ./configure.sh` if you want to use AVX2 instructions.

Then

```
$ make test
```

to run the tests. You can also run only the tests that match a chosen
regex, for example:

```
$ TEST=coord make test
```

Due to ongoing changes, benchmarks are currently broken.
