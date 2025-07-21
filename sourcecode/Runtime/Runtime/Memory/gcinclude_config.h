#pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1400) && defined(_MSC_PLATFORM_TOOLSET_llvm)
  #define GC_ATTR_MALLOC __declspec(noalias) __declspec(restrict)
#endif
#ifndef GC_DEBUG
//#define GC_DEBUG 1
#endif
#define GC_INITIAL_HEAP_SIZE 1048576
#include "gc.h"