#pragma once
#include "PWType.h"
#include "NomBuilder.h"
namespace Nom
{
	namespace Runtime
	{
		class PWTypeVar : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			PWTypeVar(llvm::Value* wrapped) : PWType(wrapped)
			{

			}
			llvm::Value* ReadIndex(NomBuilder& builder);
		};
	}
}