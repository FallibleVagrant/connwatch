#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 1

extern FILE* debug_log;

//##__VA_ARGS is gcc/clang only syntax.
#define dbgprint(fmt, ...) \
            do { if (DEBUG) fprintf(debug_log, fmt, ##__VA_ARGS__); } while (0)

#endif
