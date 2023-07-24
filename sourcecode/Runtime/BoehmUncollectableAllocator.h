#pragma once
#include <thread>
#include "boehmgcinterface.h"
#include <new>
#include "Util.h"

namespace Nom
{
	namespace Runtime
	{
		template <class T>
		struct BoehmUncollectableAllocator {
			typedef T value_type;
			BoehmUncollectableAllocator() = default;
			template <class U> constexpr BoehmUncollectableAllocator(const BoehmUncollectableAllocator<U>&) noexcept {}
			T* allocate(std::size_t n) {
				if (n > std::size_t(-1) / sizeof(T)) throw std::bad_alloc();
				if (auto p = static_cast<T*>(bgc_malloc_uncollectable(n * sizeof(T)))) return p;
				throw std::bad_alloc();
			}
			void deallocate(T* p, std::size_t) noexcept { bgc_free(remove_const(p)); }
		};
		template <class T, class U>
		bool operator==(const BoehmUncollectableAllocator<T>&, const BoehmUncollectableAllocator<U>&) { return true; }
		template <class T, class U>
		bool operator!=(const BoehmUncollectableAllocator<T>&, const BoehmUncollectableAllocator<U>&) { return false; }
	}
}
