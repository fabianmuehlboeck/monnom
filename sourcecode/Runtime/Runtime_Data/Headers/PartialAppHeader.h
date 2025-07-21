#pragma once
#include "llvm/IR/DerivedTypes.h"

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