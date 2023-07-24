#pragma once
#include "AvailableExternally.h"
#include "NomTypeDecls.h"
#include "NomBuilder.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		class RTTypeEq : public AvailableExternally<llvm::Function>
		{
		private:
			bool optimistic;
			RTTypeEq(bool optimistic);
		public:
			static RTTypeEq& Instance(bool optimistic);
			~RTTypeEq() override {}
			static llvm::FunctionType* GetLLVMFunctionType(bool optimistic);
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			static void CreateInlineTypeEqCheck(NomBuilder& builder, llvm::Value* leftType, NomTypeRef rightType, llvm::Value* leftsubstitutions, llvm::Value* rightsubstitutions, llvm::BasicBlock* pessimisticBlock, llvm::BasicBlock* optimisticBlock, llvm::BasicBlock* failBlock);
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
