void (*nissy_log)(const char *, ...);

#define LOG(...) if (nissy_log != NULL) nissy_log(__VA_ARGS__);

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
