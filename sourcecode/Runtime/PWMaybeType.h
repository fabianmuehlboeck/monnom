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
			PWMaybeType(llvm::Value* wrapped) : PWType(wrapped)
			{

			}
			PWType ReadPotentialType(NomBuilder& builder);
		};
	}
}