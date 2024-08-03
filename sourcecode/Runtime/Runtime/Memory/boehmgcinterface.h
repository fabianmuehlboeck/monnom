#pragma once
#include <memory>

namespace Nom
{
	namespace Runtime
	{
		void* bgc_malloc(size_t size);
		void* bgc_malloc_atomic(size_t size);
		void* bgc_malloc_uncollectable(size_t size);
		void bgc_register_root(char* low_address, char* high_address_plus_1);
		void bgc_free(void* mem);
	}
}