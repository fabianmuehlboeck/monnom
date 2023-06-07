#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class PWType;
		class PWTypeVar;
		class PWSubstStack : public PWrapper
		{
		public:
			static llvm::Type* GetLLVMType();
			PWSubstStack(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
			PWType Pop(NomBuilder& builder, PWTypeVar var, PWSubstStack* newStackAddr);
		};
	}
}