#include <inttypes.h>

extern "C" int64_t ctestfun2(int64_t a, int64_t b);

extern "C" void* makeBar(int64_t arg);
extern "C" int64_t getBarA(void* bar);
extern "C" int64_t compare(void* self, void* targ, void* other);

extern "C" int64_t ctestfun(void* targ, int64_t a, int64_t b){
	void* bara=makeBar(a);
	void* barb=makeBar(b);
	b = getBarA(barb) + getBarA(barb);
	return ctestfun2(compare(targ, bara, barb),b);
}
