#include "NomAlloc.h"
#include "gcinclude_config.h"

void* Nom::Runtime::gcalloc(size_t count)
{
	return GC_MALLOC(count);
}

void Nom::Runtime::gcfree(void* ref)
{
	GC_FREE(ref);
}

void* Nom::Runtime::gcalloc_atomic(size_t count)
{
	return GC_MALLOC_ATOMIC(count);
}

void* Nom::Runtime::gcalloc_uncollectable(size_t count)
{
	return GC_MALLOC_UNCOLLECTABLE(count);
}

void* Nom::Runtime::nalloc(size_t count)
{
	return gcalloc_uncollectable(count);
}

void Nom::Runtime::nfree(void* ref)
{
	gcfree(ref);
}

void* Nom::Runtime::nmalloc(size_t count)
{
	return malloc(count);
}

void Nom::Runtime::nmfree(void* ref)
{
	free(ref);
}
