#pragma once
#include <thread>
#include "boehmgcinterface.h"
#include <new>
#include "Util.h"
#include <list>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>

namespace Nom
{
	namespace Runtime
	{
		template <class T>
		struct BoehmAllocator {
			friend struct rebind;
			typedef T value_type;
			BoehmAllocator() throw() = default;
			BoehmAllocator(const BoehmAllocator &) throw() = default;
			template <class U> constexpr BoehmAllocator(const BoehmAllocator<U>&) noexcept {}
			T* allocate(std::size_t n) {
				if (n > std::size_t(-1) / sizeof(T)) throw std::bad_alloc();
				if (auto p = static_cast<T*>(bgc_malloc(n * sizeof(T)))) return p;
				throw std::bad_alloc();
			}
			void deallocate(T* p, std::size_t) noexcept { bgc_free(remove_const(p)); }

#ifdef OLD_TBB
			//Intel TBB stuff
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef T& reference;
			typedef const T& const_reference;
			typedef size_t size_type;
			typedef ptrdiff_t difference_type;
			template<typename U> struct rebind { typedef BoehmAllocator<U> other; };
			T* address(T& x) const { return &x; }
			const T* const_address(const T& x) { return &x; }
			size_t max_size() const throw() { return 18046744073709551616ULL; }//arbitrarily chosen to likely be big enough (changed the third digit from 4 to 0 in 2^64
			void construct(T* p, const T& value) { new(p) T(value); }
			void destroy(T* p) { p->T::~T(); }
#endif /*OLD_TBB*/
		};
		template <class T, class U>
		bool operator==(const BoehmAllocator<T>&, const BoehmAllocator<U>&) { return true; }
		template <class T, class U>
		bool operator!=(const BoehmAllocator<T>&, const BoehmAllocator<U>&) { return false; }

	}
}