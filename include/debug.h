#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 1

//##__VA_ARGS is gcc/clang only syntax.
#define dbgprint(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#endif
