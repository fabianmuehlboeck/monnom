#pragma once
#include "ARTRep.h"
#include "llvm/IR/Function.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomLambda;
		enum class RTLambdaFields :unsigned char { VTable = 0 };
		class RTLambda : public ARTRep<RTLambda, RTLambdaFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant(const NomLambda* lambda, llvm::Constant* interfaceMethodTable, llvm::Constant* dynamicDispatcherTable, llvm::Function* fieldRead, llvm::Function* fieldWrite);
		};
	}
}