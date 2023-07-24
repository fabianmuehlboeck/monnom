#pragma once
#include "PWrapper.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class PWType;
		class PWVMPtr;
		class PWSignature : public PWrapper
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWSignature(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}

			PWType ReadReturnType(NomBuilder& builder) const;
			PWInt32 ReadTypeParamCount(NomBuilder& builder) const;
			PWInt32 ReadParamCount(NomBuilder& builder) const;
			PWVMPtr ReadLLVMFunctionType(NomBuilder& builder) const;
			PWType ReadTypeParameter(NomBuilder& builder, PWInt32 index) const;
			PWType ReadParameter(NomBuilder& builder, PWInt32 index) const;
		};
	}
}
