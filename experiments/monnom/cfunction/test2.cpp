#include <inttypes.h>


extern "C" void* ctestfun2(void* targ, void* a, void* b, int64_t i);

extern "C" void* ctestfun(void* targ, void* a, void* b, int64_t i){
	return ctestfun2(targ,a,b,i);
}
