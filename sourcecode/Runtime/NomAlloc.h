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
#define makenmalloc(t,n) (static_cast<t *>((n)>0?(::Nom::Runtime::nmalloc(sizeof(t)*static_cast<size_t>(n))):nullptr))
#define makegcalloc(t,n) (static_cast<t *>((n)>0?(::Nom::Runtime::gcalloc(sizeof(t)*static_cast<size_t>(n))):nullptr))
#define makegcalloc_atomic(t,n) (static_cast<t *>((n)>0?(::Nom::Runtime::gcalloc_atomic(sizeof(t)*static_cast<size_t>(n))):nullptr))
#define makegcalloc_uncollectable(t,n) (static_cast<t *>((n)>0?(::Nom::Runtime::gcalloc_uncollectable(sizeof(t)*static_cast<size_t>(n))):nullptr))
