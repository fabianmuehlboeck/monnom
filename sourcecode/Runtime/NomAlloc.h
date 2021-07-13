#pragma once

#include <memory>
#include <iostream>

namespace Nom
{
	namespace Runtime
	{
		void* gcalloc(size_t count);
		void gcfree(void* ref);
		void* gcalloc_atomic(size_t count);
		void* gcalloc_uncollectable(size_t count);
		void* nalloc(size_t count);
		void nfree(void* ref);
		void* nmalloc(size_t count);
		void nmfree(void* ref);
	}
}
#define makenmalloc(t,n) ((t *)((n)>0?(::Nom::Runtime::nmalloc(sizeof(t)*(n))):nullptr))