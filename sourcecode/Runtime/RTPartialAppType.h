#pragma once
#include "llvm/IR/DerivedTypes.h"
#include "ARTRep.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTPartialAppTypeFields : unsigned char { Head = 0 };
		class RTPartialAppType : public ARTRep<RTPartialAppType, RTPartialAppTypeFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
		};
	}
}