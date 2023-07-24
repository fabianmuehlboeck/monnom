#include "gcinclude_config.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#elif defined(__GNU__)
#pragma GCC diagnostic ignored "-Wextra-semi-stmt"
#elif defined(_MSC_VER)

#endif

void gc_init()
{
	GC_INIT();
}
