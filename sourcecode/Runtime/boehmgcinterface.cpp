#include "boehmgcinterface.h"
#include "gcinclude_config.h"

namespace Nom
{
	namespace Runtime
	{
		void* bgc_malloc(size_t size)
		{
			return GC_MALLOC(size);
		}
		void* bgc_malloc_atomic(size_t size)
		{
			return GC_MALLOC_ATOMIC(size);
		}
		void* bgc_malloc_uncollectable(size_t size)
		{
			return GC_MALLOC_UNCOLLECTABLE(size);
		}
		void bgc_register_root(char* low_address, char* high_address_plus_1)
		{
			GC_add_roots(low_address, high_address_plus_1);
		}
		void bgc_free(void* mem)
		{
			GC_FREE(mem);
		}
	}
}
