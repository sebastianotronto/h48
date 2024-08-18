void (*nissy_log)(const char *, ...);

#define LOG(...) if (nissy_log != NULL) nissy_log(__VA_ARGS__);

#ifdef DEBUG
#define _static
#define _static_inline
#define DBG_WARN(condition, ...) if (!(condition)) LOG(__VA_ARGS__);
#define DBG_ASSERT(condition, retval, ...) \
    if (!(condition)) { LOG(__VA_ARGS__); return retval; }
#else
#define _static static
#define _static_inline static inline
#define DBG_WARN(condition, ...)
#define DBG_ASSERT(condition, retval, ...)
#endif
