#pragma once
#include "PWrapper.h"
#include "PWIMTFunction.h"

namespace Nom
{
	namespace Runtime
	{
		class PWDispatchPair : public PWrapper
		{
		public:
			PWDispatchPair(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			static PWDispatchPair Get(NomBuilder& builder, PWIMTFunction funptr, llvm::Value* receiver);
			PWIMTFunction GetFunction(NomBuilder& builder) const;
			llvm::Value* GetReceiver(NomBuilder& builder) const;
		};
	}
}
