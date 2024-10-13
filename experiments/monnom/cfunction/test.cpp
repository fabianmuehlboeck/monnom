#include <inttypes.h>

extern "C" int64_t ctestfun(int64_t a, int64_t b) {
	return a*(b+1);
}