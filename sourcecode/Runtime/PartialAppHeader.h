#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/DerivedTypes.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		class PartialAppHeader
		{
		public:
			static llvm::StructType* GetLLVMType();
		};
	}
}
