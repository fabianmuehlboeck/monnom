#pragma once
#include "AvailableExternally.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		class RTDisjointness : public AvailableExternally<llvm::Function>
		{
		private:
			RTDisjointness();
		public:
			static RTDisjointness& Instance();
			virtual ~RTDisjointness() override;
			static llvm::FunctionType* GetDisjointnessFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
