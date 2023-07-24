#pragma once
#include "AvailableExternally.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constant.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		class NomLambdaCallTag : public AvailableExternally<llvm::Constant>
		{
		private:
			size_t typeArgCount;
			size_t argCount;
			NomLambdaCallTag(size_t typeArgCount, size_t argCount);
		public:
			static const NomLambdaCallTag* GetCallTag(size_t typeArgCount, size_t argCount);
			~NomLambdaCallTag() override;
			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
