#include <inttypes.h>

extern "C" void* ctestfun(void* targ, void* a, void* b, int64_t i) {
	if(i<=1) {
		return a;
	}
	else {
		return b;
	}
}