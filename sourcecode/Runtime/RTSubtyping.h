#pragma once
#include "AvailableExternally.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "NomBuilder.h"
#include "NomTypeDecls.h"

namespace Nom
{
	namespace Runtime
	{
		class RTSubtyping : public AvailableExternally<llvm::Function>
		{
		private:
			RTSubtyping();
		public:
			static RTSubtyping& Instance();
			virtual ~RTSubtyping();

			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;

			static llvm::FunctionType* SubtypingFunctionType();
			static llvm::StructType* TypeArgumentListStackType();
			static llvm::Value* CreateTypeSubtypingCheck(NomBuilder& builder, llvm::Module& mod, llvm::Value* left, llvm::Value* right, llvm::Value* leftsubstitutions, llvm::Value* rightsubstitutions);

			static void CreateInlineSubtypingCheck(NomBuilder& builder, llvm::Value* leftType, llvm::Value* leftSubsts, NomTypeRef rightType, llvm::Value* rightSubstitutions, llvm::BasicBlock* pessimisticBlock, llvm::BasicBlock* optimisticBlock, llvm::BasicBlock* failBlock);
		};


	}
}
