#ifndef LOGGER_H
#define LOGGER_H

#if defined(DEVELOPMENT) && defined(VERBOSE)
#include <stdio.h>
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#define DV(type, name, val) type name = (val)
#else
#define LOG(...)
#define DV(type, name, val) val
#endif

#endif
