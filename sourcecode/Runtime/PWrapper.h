#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		template<typename T=llvm::Value*>
		class PWrapperT
		{
		public:
			T wrapped;

			PWrapperT(T _wrapped) : wrapped(_wrapped)
			{

			}
			inline T operator->() const
			{
				return wrapped;
			}
			inline operator T () const
			{
				return wrapped;
			}
		};
		using PWrapper = PWrapperT<llvm::Value*>;
	}
}
