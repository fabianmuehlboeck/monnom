#pragma once
#include "ARTRep.h"
#include "llvm/IR/Constants.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTDynamicTypeFields : unsigned char { Head = 0 };
		class RTDynamicType : public ARTRep<RTDynamicType, RTDynamicTypeFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant();
		};
	}
}