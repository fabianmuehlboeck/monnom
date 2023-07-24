#pragma once

#include "PWrapper.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Type.h"
POPDIAGSUPPRESSION
#include "NomBuilder.h"
#include "PWArr.h"

namespace Nom
{
	namespace Runtime
	{
		class PWSignature;
		class PWSuperInstance;
		class PWCastFunction;
		class PWCheckReturnValueFunction;
		class PWInterface : public PWrapper
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWInterface(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
			PWInt64 ReadSuperInstanceCount(NomBuilder& builder);
			PWInt64 ReadSuperClassCount(NomBuilder& builder);
			PWInt64 ReadSuperInterfaceCount(NomBuilder& builder);
			PWFlatArr<PWSuperInstance> ReadSuperInstances(NomBuilder& builder);
			PWFlatArr<PWSuperInstance> ReadSuperInterfaces(NomBuilder& builder);
			PWInt32 ReadTypeArgCount(NomBuilder& builder);
			PWSignature ReadSignature(NomBuilder& builder);
			PWCheckReturnValueFunction ReadReturnValueCheckFunction(NomBuilder& builder);

			llvm::Value* ReadFlags(NomBuilder& builder);
			llvm::Value* ReadIRPtr(NomBuilder& builder);
			PWCastFunction ReadCastFunction(NomBuilder& builder);
		};
	}
}
