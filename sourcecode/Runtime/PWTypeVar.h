#pragma once
#include "PWType.h"
#include "NomBuilder.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class PWTypeVar : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWTypeVar(llvm::Value* _wrapped) : PWType(_wrapped)
			{

			}
			PWInt32 ReadIndex(NomBuilder& builder);
		};
	}
}
