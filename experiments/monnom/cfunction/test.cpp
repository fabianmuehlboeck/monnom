#include <inttypes.h>

extern "C" int64_t ctestfun2(int64_t a, int64_t b);

extern "C" int64_t ctestfun(int64_t a, int64_t b){
	return ctestfun2(a,b);
}

