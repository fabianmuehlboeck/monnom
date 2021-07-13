#pragma once
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{
		class RTTypeEq : public AvailableExternally<llvm::Function>
		{
		private:
			RTTypeEq();
		public:
			static RTTypeEq& Instance();
			static llvm::FunctionType* GetLLVMFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}