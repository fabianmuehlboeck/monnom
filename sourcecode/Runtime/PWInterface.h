#pragma once

#include "PWrapper.h"
#include "llvm/IR/Type.h"
#include "NomBuilder.h"

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
			PWInterface(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
			llvm::Value* ReadSuperInstanceCount(NomBuilder& builder);
			llvm::Value* ReadSuperClassCount(NomBuilder& builder);
			llvm::Value* ReadSuperInterfaceCount(NomBuilder& builder);
			PWSuperInstance ReadSuperInstances(NomBuilder& builder);
			PWSuperInstance ReadSuperInterfaces(NomBuilder& builder);
			llvm::Value* ReadTypeArgCount(NomBuilder& builder);
			PWSignature ReadSignature(NomBuilder& builder);
			PWCheckReturnValueFunction ReadReturnValueCheckFunction(NomBuilder& builder);

			llvm::Value* ReadFlags(NomBuilder& builder);
			llvm::Value* ReadIRPtr(NomBuilder& builder);
			PWCastFunction ReadCastFunction(NomBuilder& builder);
		};
	}
}