#pragma once

#include <memory>
#include <list>
#include "llvm/ADT/SmallVector.h"

#ifndef RECURSION_BUFFER_SIZE
#define RECURSION_BUFFER_SIZE 8
#endif // !RECURSION_BUFFER_SIZE

#ifndef RECURSION_BUFFER_PAGESIZE
#define RECURSION_BUFFER_PAGESIZE 4096
#endif // !RECURSION_BUFFER_PAGESIZE

#ifndef RECURSION_BUFFER_ALIGNMENT
#define RECURSION_BUFFER_ALIGNMENT 4;
#endif // !RECURSION_BUFFER_ALIGNMENT


namespace Nom
{
	namespace Runtime
	{
		class RecursionBuffer
		{
		private:
			char* pages[RECURSION_BUFFER_SIZE] = { 0 };
			size_t pageCount = 0;
			size_t depth = 0;
			char * current = nullptr;
			size_t bytes_left = 0;
			void * Bump()
			{
				if (pageCount >= RECURSION_BUFFER_SIZE)
				{
					throw pageCount; //TODO: correct exception
				}
				current = (char *)malloc(RECURSION_BUFFER_PAGESIZE);
				pages[pageCount] = current;
				bytes_left = RECURSION_BUFFER_PAGESIZE;
				pageCount += 1;
				return current;
			}

			RecursionBuffer()
			{
				Bump();
			}
		public:
			static RecursionBuffer& Instance() {
				static thread_local RecursionBuffer instance; return instance;
			};
			~RecursionBuffer()
			{
				for (; pageCount > 0; pageCount--)
				{
					free(pages[pageCount - 1]);
				}
			}

			void Enter()
			{
				depth += 1;
			}

			void * alloc(size_t size)
			{
				if (size <= 0)
				{
					return nullptr;
				}
				if (size > RECURSION_BUFFER_PAGESIZE)
				{
					throw size; //TODO: correct exception
				}
				while (size > bytes_left)
				{
					Bump();
				}
				void * ret = (void*) current;
				current += size;
				current += ((intptr_t)current) % RECURSION_BUFFER_ALIGNMENT;
				bytes_left -= ((intptr_t)current) - ((intptr_t)ret);
				return ret;
			}

			void Exit()
			{
				depth -= 1;
				if (depth == 0)
				{
					for (; pageCount > 1; pageCount--)
					{
						free(pages[pageCount - 1]);
					}
					current = pages[0];
					bytes_left = RECURSION_BUFFER_PAGESIZE;
				}
			}
		};

		template <class T>
		struct RecursionBufferAllocator {
			typedef T value_type;
			RecursionBufferAllocator() = default;
			template <class U> constexpr RecursionBufferAllocator(const RecursionBufferAllocator<U>&) noexcept {}
			T* allocate(std::size_t n) {
				if (n > std::size_t(-1) / sizeof(T)) throw std::bad_alloc();
				if (auto p = static_cast<T*>(RecursionBuffer::Instance().alloc(n * sizeof(T)))) return p;
				throw std::bad_alloc();
			}
			void deallocate(T* p, std::size_t) noexcept {  }
			template<typename _T1>
			struct rebind
			{
				typedef RecursionBufferAllocator<_T1> other;
			};
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef T& reference;
			typedef const T& const_reference;
			void construct(pointer p, const_reference val)
			{
				new((void *)p) T(val);
			}
			template< class U, class... Args >
			static void construct(RecursionBufferAllocator<U>& a, U* p, Args&&... args)
			{
				::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
			}
		};
		template <class T, class U>
		bool operator==(const RecursionBufferAllocator<T>&, const RecursionBufferAllocator<U>&) { return true; }
		template <class T, class U>
		bool operator!=(const RecursionBufferAllocator<T>&, const RecursionBufferAllocator<U>&) { return false; }

		class NomType;
		using RecBufferTypeList = llvm::SmallVector<NomType *, 16>;
		using InstantiationList = std::list<RecBufferTypeList, RecursionBufferAllocator<RecBufferTypeList>>;

	}
}

