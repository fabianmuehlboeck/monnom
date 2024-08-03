#pragma once
#include "llvm/ADT/ArrayRef.h"

namespace Nom
{
	namespace Runtime
	{
		template<typename T> T& remove_const(const T&t) { return const_cast<T&>(t); }

		template<typename T> llvm::ArrayRef<T> cloneArrayRef(const llvm::ArrayRef<T> &arr) {
			size_t size = arr.size();
			T *ret = (T*) malloc(sizeof(T) * size);
			for (int i = size; i > 0;)
			{
				i--;
				ret[i] = arr[i];
			}
			return llvm::ArrayRef<T>(ret, size);
		}
	}
}