#pragma once
#include "llvm/IR/Value.h"

namespace Nom
{
	namespace Runtime
	{
		class PWrapper
		{
		public:
			llvm::Value* wrapped;

			PWrapper(llvm::Value* wrapped) : wrapped(wrapped)
			{

			}
			inline llvm::Value* operator->() const
			{
				return wrapped;
			}
			inline operator llvm::Value* () const
			{
				return wrapped;
			}
		};
	}
}