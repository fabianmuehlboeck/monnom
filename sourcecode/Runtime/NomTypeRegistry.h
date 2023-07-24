#pragma once
#include <unordered_map>
PUSHDIAGSUPPRESSION
#include <llvm/IR/Constant.h>
#include <llvm/ADT/SmallVector.h>
POPDIAGSUPPRESSION
#include "RTTypeHead.h"
#include "NomClass.h"
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{

		class TypeRegistry : public AvailableExternally<llvm::Function>
		{
		private:
			TypeRegistry();
		public:
			static TypeRegistry& Instance();
			virtual ~TypeRegistry() override;

			static llvm::FunctionType* GetLLVMFunctionType();

			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
