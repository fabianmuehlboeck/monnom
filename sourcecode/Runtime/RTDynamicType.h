#pragma once
#include "ARTRep.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constants.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		enum class RTDynamicTypeFields : unsigned char { Head = 0 };
		class RTDynamicType : public ARTRep<RTDynamicType, RTDynamicTypeFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant(llvm::Constant* castFun);
		};
	}
}
