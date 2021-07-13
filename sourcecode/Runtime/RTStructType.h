#pragma once
#include "llvm/IR/DerivedTypes.h"
#include "ARTRep.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTStructTypeFields : unsigned char { Head = 0 };
		class RTStructType : public ARTRep<RTStructType, RTStructTypeFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
		};
	}
}