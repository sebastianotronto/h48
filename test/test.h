#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRLENMAX 10000

#ifdef CUBE_AVX2
#include <immintrin.h>
typedef __m256i cube_t;
#else
typedef struct {
	uint8_t corner[8];
	uint8_t edge[12];
} cube_t;
#endif

/* Basic functions used in most tests */
cube_t solvedcube(void);
bool iserror(cube_t);
bool isconsistent(cube_t);
bool issolvable(cube_t);
bool issolved(cube_t);
cube_t readcube(char *, char *);
void writecube(char *, cube_t, char *);
void nissy_setlogger(void (*logger_function)(const char *, va_list));

/* Test function to be implemented by all tests */
void run(void);

void log_stderr(const char *str, va_list vl) { vfprintf(stderr, str, vl); }

int main(void) {
	nissy_setlogger(log_stderr);
	run();
	return 0;
}
