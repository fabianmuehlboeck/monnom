#pragma once
#include "AvailableExternally.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

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
			virtual ~RTDisjointness();
			static llvm::FunctionType* GetDisjointnessFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}