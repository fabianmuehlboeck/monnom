#pragma once
#include "PWTypeArr.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class PWInterface;
		class PWClassType : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWClassType(llvm::Value* _wrapped) : PWType(_wrapped)
			{

			}
			PWInterface ReadClassDescriptorLink(NomBuilder& builder);
			PWTypeArr TypeArgumentsPointer(NomBuilder& builder);
		};
	}
}
