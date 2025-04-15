#include <stdio.h>

void (*nissy_log)(const char *, void *);
void *nissy_log_data;
void write_wrapper(void (*)(const char *, void *), const char *, ...);

void
write_wrapper(void (*write)(const char *, void *), const char *str, ...)
{
	static const size_t len = 1000;
	char message[len];
	va_list args;

	va_start(args, str);
	vsprintf(message, str, args);
	va_end(args);

	write(message, nissy_log_data);
}

#define LOG(...) if (nissy_log != NULL) write_wrapper(nissy_log, __VA_ARGS__);

#ifdef DEBUG
#define STATIC
#define STATIC_INLINE
#define DBG_WARN(condition, ...) if (!(condition)) LOG(__VA_ARGS__);
#define DBG_ASSERT(condition, retval, ...) \
    if (!(condition)) { LOG(__VA_ARGS__); return retval; }
#else
#define STATIC static
#define STATIC_INLINE static inline
#define DBG_WARN(condition, ...)
#define DBG_ASSERT(condition, retval, ...)
#endif
