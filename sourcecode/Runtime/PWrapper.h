#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION
#include "NomBuilder.h"

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
			inline bool IsEmpty() const
			{
				return wrapped == nullptr;
			}
		};
		using PWrapper = PWrapperT<llvm::Value*>;
	}
}
