#pragma once
#include "PWType.h"
#include "NomBuilder.h"
namespace Nom
{
	namespace Runtime
	{
		class PWMaybeType : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWMaybeType(llvm::Value* _wrapped) : PWType(_wrapped)
			{

			}
			PWType ReadPotentialType(NomBuilder& builder);
		};
	}
}
