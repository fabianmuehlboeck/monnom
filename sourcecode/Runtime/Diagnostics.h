#pragma once

#if defined(__clang__)
#define PUSHDIAGSUPPRESSION _Pragma("clang diagnostic push") \
 _Pragma("clang diagnostic ignored \"-Weverything\"")
#define POPDIAGSUPPRESSION _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define PUSHDIAGSUPPRESSION _Pragma("GCC diagnostic push") \
 _Pragma("GCC diagnostic ignored \"-Wall\"")
#define POPDIAGSUPPRESSION _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#define PUSHDIAGSUPPRESSION __pragma(warning(push,0))
#define POPDIAGSUPPRESSION __pragma(warning(pop))
#else
#define PUSHDIAGSUPPRESSION
#define POPDIAGSUPPRESSION
#endif
