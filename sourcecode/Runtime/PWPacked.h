#pragma once
#include "PWrapper.h"

namespace Nom
{
	namespace Runtime
	{
		class PWPacked : public PWrapper
		{
		public:
			PWPacked(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
			llvm::Value* ReadTypeTag(NomBuilder& builder) const;
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
		};
	}
}
