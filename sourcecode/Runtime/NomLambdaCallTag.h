#pragma once
#include "AvailableExternally.h"
#include "llvm/IR/Constant.h"

namespace Nom
{
	namespace Runtime
	{
		class NomLambdaCallTag : public AvailableExternally<llvm::Constant>
		{
		private:
			int typeArgCount;
			int argCount;
			NomLambdaCallTag(int typeArgCount, int argCount);
		public:
			static const NomLambdaCallTag* GetCallTag(int typeArgCount, int argCount);
			~NomLambdaCallTag();
			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}