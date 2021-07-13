#include <limits.h>
#include <inttypes.h>
#include <stdint.h>

#ifndef NG_INT
#if INTPTR_MAX > 2147483648
#define NG_INT intptr_t
#define NG_FLOAT double
#define NG_X64
#else
#define NG_X86
#define NG_INT intptr_t
#define NG_FLOAT float
#endif

typedef size_t NG_CONTEXTID;
typedef NG_CONTEXTID NG_CONTEXT;
typedef uintptr_t NG_ACCESSLEVEL;
#endif

