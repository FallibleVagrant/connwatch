#ifndef COMMON_H
#define COMMON_H

//Cf. note in main.cpp about SIGWINCH.
//extern volatile int should_resize;

//Animation stuff.
extern unsigned int ticker;

#include <stdlib.h>

//Crash if OOM.
void* kalloc(size_t nmemb, size_t size);

#include <stdio.h>

extern FILE* error_log;

//##__VA_ARGS is gcc/clang only syntax.
#define errprint(fmt, ...) \
            do { fprintf(error_log, fmt, ##__VA_ARGS__); fflush(error_log); } while (0)

#endif
