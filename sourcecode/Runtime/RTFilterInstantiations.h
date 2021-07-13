#pragma once
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{
		class RTFilterInstantiations : public AvailableExternally<llvm::Function>
		{
		private:
			RTFilterInstantiations();
		public:
			static RTFilterInstantiations &Instance();
			virtual ~RTFilterInstantiations();
			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}