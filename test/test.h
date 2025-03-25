#define TEST_H

#include <inttypes.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/nissy.h"
#include "../src/arch/arch.h"
#include "../src/solvers/solutions_types_macros.h"
#include "../src/solvers/tables_types_macros.h"
#include "../src/solvers/h48/coordinate_types_macros.h"
#include "../src/solvers/h48/map_types_macros.h"
#include "../src/solvers/h48/gendata_types_macros.h"
#include "../src/solvers/coord/types_macros.h"

#define STRLENMAX 10000

/* Basic functions used in most tests */
cube_t solvedcube(void);
bool iserror(cube_t);
bool isconsistent(cube_t);
bool issolvable(cube_t);
bool issolved(cube_t);
cube_t readcube(char *, char *);
int64_t writecube(char *, cube_t, size_t n, char [n]);

/* Test function to be implemented by all tests */
void run(void);

void log_stderr(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	vfprintf(stderr, str, args);
	va_end(args);
}

int main(void) {
	nissy_setlogger(log_stderr);
	run();
	return 0;
}
