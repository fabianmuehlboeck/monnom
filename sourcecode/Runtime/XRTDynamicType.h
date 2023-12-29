#pragma once
#include "ARTRep.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constants.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		enum class XRTDynamicTypeFields : unsigned char { Head = 0 };
		class XRTDynamicType : public ARTRep<XRTDynamicType, XRTDynamicTypeFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant(llvm::Constant* castFun);
		};
	}
}
