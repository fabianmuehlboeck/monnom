#pragma once
#include "llvm/IR/Function.h"
#include "llvm/IR/DerivedTypes.h"
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{
		class NomCallable;
		class NomCallableVersion : public AvailableExternally<llvm::Function>
		{
		private:
			static size_t cvcounter();
			size_t cvid;
		public:
			const NomCallable* const Callable;
			llvm::FunctionType* const FunType;
			NomCallableVersion(const NomCallable* callable, llvm::FunctionType* functionType);
			~NomCallableVersion() = default;

			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}